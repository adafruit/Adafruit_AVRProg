// Useful message printing definitions
#include "Adafruit_AVRProg.h"
#include "Adafruit_UPDIProg.h"

#if defined(__AVR__)         // smol mem
static byte pageBuffer[128]; /* One page of flash */
#else
static byte pageBuffer[8 * 1024]; // we can megabuff
#endif

static bool isUPDIchip = false;

/**************************************************************************/
/*!
    @brief  Instantiate a new programmer, no pins are defined to start
*/
/**************************************************************************/
Adafruit_AVRProg::Adafruit_AVRProg() {
  progLED = errLED = -1;
  _sck = _miso = _mosi = _reset = -1;
  spi = NULL;
  programmode = false;
  spiBitDelay = 0;
}

/**************************************************************************/
/*!
    @brief  Configure a hardware SPI interface, plus one reset pin
    @param    reset_pin The arduino pin connected to the target RESET
    @param    theSPI an SPIClass connected to the ISP port of the AVR
*/
/**************************************************************************/
void Adafruit_AVRProg::setSPI(int8_t reset_pin, SPIClass *theSPI) {
  _reset = reset_pin;
  spi = theSPI;
  isUPDIchip = false;
}

/**************************************************************************/
/*!
    @brief  Configure a software SPI interface, plus one reset pin
    @param    reset_pin The arduino pin connected to the target RESET
    @param    sck_pin The arduino pin connected to the target SCK
    @param    mosi_pin The arduino pin connected to the target MOSI

    @param    miso_pin The arduino pin connected to the target MISO
*/
/**************************************************************************/
void Adafruit_AVRProg::setSPI(int8_t reset_pin, int8_t sck_pin, int8_t mosi_pin,
                              int8_t miso_pin) {
  _reset = reset_pin;
  _mosi = mosi_pin;
  _miso = miso_pin;
  _sck = sck_pin;
  isUPDIchip = false;
}

/**************************************************************************/
/*!
    @brief  Set up programming to use UPDI interface
    @param  theSerial Hardware serial interface (CANNOT use software serial!)
    @param  baudrate Try 115200 or less to start
    @param  power_pin pin connected to target chip power (can help with UPDI
   since theres no reset line)
    @param invertpower Set true if the power pin is low to enable target
*/
/**************************************************************************/
void Adafruit_AVRProg::setUPDI(HardwareSerial *theSerial, uint32_t baudrate,
                               int8_t power_pin, bool invertpower) {
  uart = theSerial;
  _baudrate = baudrate;
  _power = power_pin;
  _invertpower = invertpower;
  isUPDIchip = true;
}

/**************************************************************************/
/*!
    @brief  End any SPI transactions, set reset pin to input with no pullup
*/
/**************************************************************************/
void Adafruit_AVRProg::endProgramMode(void) {
  if (spi) {
    spi->endTransaction();
    spi->end();
  } else {
    pinMode(_miso, INPUT);
    pinMode(_mosi, INPUT);
    pinMode(_sck, INPUT);
  }
  digitalWrite(_reset, LOW);
  pinMode(_reset, INPUT);

  programmode = false;
}

/**************************************************************************/
/*!
    @brief    Begin/end programming, turn on/off a programming LED if we have
   one
    @param    poweron Whether we are starting or ending programming
    @return True if we were able to connect to an AVR target on start. Always
    true on end programming.
*/
/**************************************************************************/
bool Adafruit_AVRProg::targetPower(bool poweron) {
  if (poweron) {
    if (progLED >= 0) {
      pinMode(progLED, OUTPUT);
      digitalWrite(progLED, HIGH);
    }
    Serial.print(F("Starting Program Mode..."));
    if (startProgramMode()) {
      Serial.println(F(" [OK]"));
      return true;
    } else {
      Serial.println(F(" [FAIL]"));
      return false;
    }
  } else {
    endProgramMode();
    if (progLED >= 0) {
      digitalWrite(progLED, LOW);
    }
    return true;
  }
}

bool Adafruit_AVRProg::startProgramMode(uint32_t clockspeed) {
  if (uart) { // its UPDI time!
#ifndef SUPPORT_UPDI
    return false; // not supported on this platform :(
#else
    updi_init(true);
    if (!updi_check()) {
      DEBUG_VERBOSE("UPDI not initialised\n");

      if (!updi_device_force_reset()) {
        DEBUG_VERBOSE("double BREAK reset failed\n");
        return false;
      }
      updi_init(false); // re-init the UPDI interface

      if (!updi_check()) {
        DEBUG_VERBOSE("Cannot initialise UPDI, aborting.\n");
        // TODO find out why these are not already correct
        g_updi.initialized = false;
        g_updi.unlocked = false;
        return false;
      } else {
        DEBUG_VERBOSE("UPDI INITIALISED\n");
        g_updi.initialized = true;
      }
    }
    return true;
#endif
  } else {
    if (_reset >= 0) {
      pinMode(_reset, OUTPUT);
      digitalWrite(_reset, HIGH);
      delay(5);
    }

    if (spi) {
      debug(F("Using hardware SPI"));
      spi->begin();
      spi->beginTransaction(SPISettings(clockspeed, MSBFIRST, SPI_MODE0));
    } else if ((_sck >= 0) && (_mosi >= 0) && (_miso >= 0)) {
      debug(F("Using software SPI"));
      pinMode(_sck, OUTPUT);
      digitalWrite(_sck, LOW);
      pinMode(_miso, INPUT);
      pinMode(_mosi, OUTPUT);
      float _delay = 1000.0 * 1000.0 / (float)clockspeed;
      // Serial.println(_delay);
      spiBitDelay = _delay;
    } else {
      error(F("Neither hardware or software SPI modes selected"));
    }
    debug("...spi_init done");

    if (_reset >= 0) {
      digitalWrite(_reset, LOW);
    }

    debug("...isp_transaction");
    uint16_t reply = isp_transaction(0xAC, 0x53, 0x00, 0x00);
    if (reply == 0x5300) {
      debug("...Done");
      programmode = true;
      return true;
    }
    Serial.print(reply, HEX);
    return false;
  }
}

/*******************************************************
 * ISP high level commands
 */

/**************************************************************************/
/*!
    @brief  Read the bottom two signature bytes (if possible) and return them
    Note that the highest signature byte is the same over all AVRs so we skip it
    @returns The two bytes as one uint16_t
*/
/**************************************************************************/
uint16_t Adafruit_AVRProg::readSignature(void) {
  if (uart) {
#ifndef SUPPORT_UPDI
    return 0x0; // not supported on this platform :(
#else
    updi_run_tasks(UPDI_TASK_GET_INFO, NULL);

    uint16_t sig = 0;
    sig = g_updi.details.signature_bytes[1];
    sig <<= 8;
    sig |= g_updi.details.signature_bytes[2];

    return sig;
#endif
  } else {
    // SPI mode
    startProgramMode(FUSE_CLOCKSPEED);

    uint16_t target_type = 0;

    target_type = isp_transaction(0x30, 0x00, 0x01, 0x00);
    target_type <<= 8;
    target_type |= isp_transaction(0x30, 0x00, 0x02, 0x00);

    endProgramMode();

    return target_type;
  }
}

/**************************************************************************/
/*!
    @brief    Send the erase command, then busy wait until the chip is erased
    @returns  True if erase command succeeds
*/
/**************************************************************************/
bool Adafruit_AVRProg::eraseChip(void) {
  if (uart) {
#ifndef SUPPORT_UPDI
    return false; // not supported on this platform :(
#else
    return updi_run_tasks(UPDI_TASK_ERASE, NULL);
#endif
  } else {
    startProgramMode(FUSE_CLOCKSPEED);
    if ((isp_transaction(0xAC, 0x80, 0, 0) & 0xFFFF) != 0x8000) { // chip erase
      error(F("Error on chip erase command"));
    }
    busyWait();
    endProgramMode();
    return true;
  }
}

/**************************************************************************/
/*!
    @brief    Read the fuses on a device
    @param    fuses Pointer to 4-byte array of fuses
    @param    numbytes How many fuses to read (UPDI has 10?)
    @return True if we were able to send data and get a response from the chip.
    You could still run verifyFuses() afterwards!
*/
/**************************************************************************/
bool Adafruit_AVRProg::readFuses(byte *fuses, uint8_t numbytes) {
  (void)fuses[0];
  (void)numbytes;

  if (uart) {
#ifndef SUPPORT_UPDI
    return false; // not supported on this platform :(
#else
    if (!updi_run_tasks(UPDI_TASK_READ_FUSES, NULL)) {
      return false;
    }
    for (uint8_t i = 0; i < numbytes; i++) {
      fuses[i] = g_updi.fuses[i];
    }
    return true;
#endif
  } else {
    // todo: SPI!
    return false;
  }
}

/**************************************************************************/
/*!
    @brief    Program the fuses on a device
    @param    fuses Pointer to byte array of fuses
    @param    num_fuses How many fuses are in the fusearray
    @return True if we were able to send data and get a response from the chip.
    You could still run verifyFuses() afterwards!
*/
/**************************************************************************/
bool Adafruit_AVRProg::programFuses(const byte *fuses, uint8_t num_fuses) {
  startProgramMode(FUSE_CLOCKSPEED);

  if (uart) {
#ifndef SUPPORT_UPDI
    return false; // not supported on this platform :(
#else

    uint8_t old_fuses[10];
    if (!updi_run_tasks(UPDI_TASK_READ_FUSES, NULL)) {
      return false;
    }
    Serial.print("Old fuses: ");
    for (uint8_t i = 0; i < num_fuses; i++) {
      old_fuses[i] = g_updi.fuses[i];
      Serial.print("0x");
      Serial.print(old_fuses[i], HEX);
      Serial.print(", ");
    }

    for (uint8_t f = 0; f < num_fuses; f++) {
      if (f == (AVR_FUSE_LOCK - AVR_FUSE_BASE)) {
        Serial.println(
            "Nope: we are not going to let you change the lock bits\n");
        continue;
      }

      g_updi.fuses[f] = fuses[f];
    }

    if (!updi_run_tasks(UPDI_TASK_WRITE_FUSES, NULL)) {
      return false;
    }

    return true;
#endif
  } else {

    byte f;
    Serial.println(F("\nSetting fuses"));

    f = pgm_read_byte(&fuses[FUSE_PROT]);
    if (f) {
      Serial.print(F("\tSet Lock Fuse to: "));
      Serial.println(f, HEX);
      if ((isp_transaction(0xAC, 0xE0, 0x00, f) & 0xFFFF) != 0xE000) {
        return false;
      }
    }
    busyWait();
    f = pgm_read_byte(&fuses[FUSE_LOW]);
    if (f) {
      Serial.print(F("\tSet Low Fuse to: "));
      Serial.println(f, HEX);
      if ((isp_transaction(0xAC, 0xA0, 0x00, f) & 0xFFFF) != 0xA000) {
        return false;
      }
    }
    busyWait();
    f = pgm_read_byte(&fuses[FUSE_HIGH]);
    if (f) {
      Serial.print(F("\tSet High Fuse to: "));
      Serial.println(f, HEX);
      if ((isp_transaction(0xAC, 0xA8, 0x00, f) & 0xFFFF) != 0xA800) {
        return false;
      }
    }
    busyWait();
    f = pgm_read_byte(&fuses[FUSE_EXT]);
    if (f) {
      Serial.print(F("\tSet Ext Fuse to: "));
      Serial.println(f, HEX);
      if ((isp_transaction(0xAC, 0xA4, 0x00, f) & 0xFFFF) != 0xA400) {
        return false;
      }
    }
    busyWait();
    Serial.println();
    endProgramMode();
    return true;
  }
}

/**************************************************************************/
/*!
    @brief    Program a single fuse (currently for UPDI only)
    @param    fuse Value to write
    @param    num Fuse address offset (start at 0)
    @return  UPDI command success status
*/
/**************************************************************************/
bool Adafruit_AVRProg::programFuse(byte fuse, uint8_t num) {
  if (uart) {
#ifndef SUPPORT_UPDI
    return false; // not supported on this platform :(
#else

    uint8_t old_fuses[10];
    if (!updi_run_tasks(UPDI_TASK_READ_FUSES, NULL)) {
      return false;
    }
    Serial.print("Old fuses: ");
    for (uint8_t i = 0; i < 10; i++) {
      old_fuses[i] = g_updi.fuses[i];
      Serial.print("0x");
      Serial.print(old_fuses[i], HEX);
      Serial.print(", ");
    }

    if (num == (AVR_FUSE_LOCK - AVR_FUSE_BASE)) {
      Serial.println(
          "Nope: we are not going to let you change the lock bits\n");
      return false;
    }

    g_updi.fuses[num] = fuse;

    Serial.printf("\nWriting fuse #%d -> 0x%02x\n", num, fuse);
    if (!updi_run_tasks(UPDI_TASK_WRITE_FUSES, NULL)) {
      return false;
    }

    return true;
#endif
  } else {
    return false;
  }
}

/**************************************************************************/
/*!
    @brief    Verify the fuses are set to what we expect
    @param    fuses Pointer to 4-byte array of fuses
    @param    fusemask Pointer to 4-byte array of bitmasks to apply to fuses
    that are read.
    @return True if the fuses on the device are identical to the fuses passed in
*/
/**************************************************************************/
boolean Adafruit_AVRProg::verifyFuses(const byte *fuses, const byte *fusemask) {
  startProgramMode(FUSE_CLOCKSPEED);

  byte f;
  Serial.println(F("Verifying fuses..."));
  f = pgm_read_byte(&fuses[FUSE_PROT]);
  if (f) {
    uint8_t readfuse = isp_transaction(0x58, 0x00, 0x00, 0x00); // lock fuse
    readfuse &= pgm_read_byte(&fusemask[FUSE_PROT]);
    Serial.print(F("\tLock Fuse = 0x"));
    Serial.println(readfuse, HEX);
    if (readfuse != f)
      return false;
  }
  f = pgm_read_byte(&fuses[FUSE_LOW]);
  if (f) {
    uint8_t readfuse = isp_transaction(0x50, 0x00, 0x00, 0x00); // low fuse
    Serial.print(F("\tLow Fuse = 0x"));
    Serial.println(readfuse, HEX);
    readfuse &= pgm_read_byte(&fusemask[FUSE_LOW]);
    if (readfuse != f)
      return false;
  }
  f = pgm_read_byte(&fuses[FUSE_HIGH]);
  if (f) {
    uint8_t readfuse = isp_transaction(0x58, 0x08, 0x00, 0x00); // high fuse
    readfuse &= pgm_read_byte(&fusemask[FUSE_HIGH]);
    Serial.print(F("\tHigh Fuse = 0x"));
    Serial.println(readfuse, HEX);
    if (readfuse != f)
      return false;
  }
  f = pgm_read_byte(&fuses[FUSE_EXT]);
  if (f) {
    uint8_t readfuse = isp_transaction(0x50, 0x08, 0x00, 0x00); // ext fuse
    readfuse &= pgm_read_byte(&fusemask[FUSE_EXT]);
    Serial.print(F("\tExt Fuse = 0x"));
    Serial.println(readfuse, HEX);
    if (readfuse != f)
      return false;
  }
  Serial.println();
  endProgramMode();
  return true;
}

/**************************************************************************/
/*!
    @brief    Program the chip flash
    @param    hextext A pointer to a array of bytes in INTEL HEX format
    @param    pagesize The flash-page size of this chip, in bytes. Check
   datasheet!
    @param    chipsize The flash size of this chip, in bytes. Check datasheet!
    @return True if flashing worked out, check the data with verifyImage!
*/
/**************************************************************************/
bool Adafruit_AVRProg::writeImage(const byte *hextext, uint32_t pagesize,
                                  uint32_t chipsize) {
  uint32_t flash_start = 0;
#ifdef SUPPORT_UPDI
  if (isUPDIchip) {
    flash_start = g_updi.config->flash_start;
  }
#endif

  uint32_t pageaddr = 0;

#if VERBOSE
  Serial.print(F("Chip size: "));
  Serial.println(chipsize, DEC);
  Serial.print(F("Page size: "));
  Serial.println(pagesize, DEC);
  Serial.print(F("Flash start: "));
  Serial.println(flash_start, DEC);
#endif

  while (pageaddr < chipsize && hextext) {
    const byte *hextextpos =
        readImagePage(hextext, pageaddr, pagesize, pageBuffer);

    bool blankpage = true;
    for (uint16_t i = 0; i < pagesize; i++) {
      if (pageBuffer[i] != 0xFF)
        blankpage = false;
    }
    if (!blankpage) {
      if (!flashPage(pageBuffer, flash_start + pageaddr, pagesize))
        return false;
    }
    hextext = hextextpos;
    pageaddr += pagesize;
  }
  return true;
}

/*
 * readImagePage
 *
 * Read a page of intel hex image from a string in pgm memory. Returns a pointer
 * to where we ended
 */

const byte *Adafruit_AVRProg::readImagePage(const byte *hextext,
                                            uint16_t pageaddr,
                                            uint16_t pagesize, byte *page) {
  uint16_t len;
  uint16_t page_idx = 0;
  const byte *beginning = hextext;

  byte b, cksum = 0;

  // 'empty' the page by filling it with 0xFF's
  for (uint16_t i = 0; i < pagesize; i++)
    page[i] = 0xFF;

  while (1) {
    uint16_t lineaddr;
    char c;

    // read one line!
    c = pgm_read_byte(hextext++);
    if (c == '\n' || c == '\r') {
      continue;
    }
    if (c != ':') {
      error(F(" No colon?"));
      break;
    }
    // Read the byte count into 'len'
    len = hexToByte(pgm_read_byte(hextext++));
    len = (len << 4) + hexToByte(pgm_read_byte(hextext++));
    cksum = len;
    // Serial.print(len);

    // read high address byte
    b = hexToByte(pgm_read_byte(hextext++));
    b = (b << 4) + hexToByte(pgm_read_byte(hextext++));
    cksum += b;
    lineaddr = b;

    // read low address byte
    b = hexToByte(pgm_read_byte(hextext++));
    b = (b << 4) + hexToByte(pgm_read_byte(hextext++));
    cksum += b;
    lineaddr = (lineaddr << 8) + b;

    if (lineaddr >= (pageaddr + pagesize)) {
      return beginning;
    }

    b = hexToByte(pgm_read_byte(hextext++)); // record type
    b = (b << 4) + hexToByte(pgm_read_byte(hextext++));
    cksum += b;
    // Serial.print("Record type "); Serial.println(b, HEX);
    if (b == 0x1) {
      // end record, return nullptr to indicate we're done
      hextext = nullptr;
      break;
    }
#if VERBOSE > 2
    Serial.print(F("\nLine address =  0x"));
    Serial.print(lineaddr, HEX);
    Serial.print(F(", Page address =  0x"));
    Serial.println(pageaddr, HEX);
    Serial.print(F("HEX data: "));
#endif
    for (byte i = 0; i < len; i++) {
      // read 'n' bytes
      b = hexToByte(pgm_read_byte(hextext++));
      b = (b << 4) + hexToByte(pgm_read_byte(hextext++));

      cksum += b;
#if VERBOSE > 2
      Serial.print(b, HEX);
      Serial.write(' ');
#endif

      page[page_idx] = b;
      page_idx++;

      if (page_idx > pagesize) {
        error("Too much code!");
        break;
      }
    }
    b = hexToByte(pgm_read_byte(hextext++)); // chxsum
    b = (b << 4) + hexToByte(pgm_read_byte(hextext++));
    cksum += b;
    if (cksum != 0) {
      error(F("Bad checksum: "));
      Serial.print(cksum, HEX);
    }
    if (pgm_read_byte(hextext++) != '\n') {
      error(F("No end of line"));
      break;
    }
#if VERBOSE
    Serial.println();
    Serial.print(F("Page index: "));
    Serial.println(page_idx, DEC);
#endif
    if (page_idx == pagesize)
      break;
  }
#if VERBOSE
  Serial.print(F("\n  Total bytes read: "));
  Serial.println(page_idx, DEC);
#endif
  return hextext;
}

/**************************************************************************/
/*!
    @brief    Does a byte-by-byte verify of the flash hex against the chip
    Thankfully this does not have to be done by pages!
    @param    hextext A pointer to a array of bytes in INTEL HEX format
    @return True if the image is the same as the hextext, returns false on any
    error
*/
/**************************************************************************/
bool Adafruit_AVRProg::verifyImage(const byte *hextext) {
  if (uart) {
#ifndef SUPPORT_UPDI
    return false; // not supported on this platform :(
#else
    uint32_t pageaddr = 0;
    // uint16_t pagesize = g_updi.config->flash_pagesize;
    uint16_t pagebuffersize = AVR_PAGESIZE_MAX;
    uint16_t chipsize = g_updi.config->flash_size;
    uint8_t buffer1[pagebuffersize], buffer2[pagebuffersize];

#if VERBOSE
    Serial.print(F("Chip size: "));
    Serial.println(chipsize, DEC);
    Serial.print(F("Buffer size: "));
    Serial.println(pagebuffersize, DEC);
#endif

    while ((pageaddr < chipsize) && hextext) {
      const byte *hextextpos =
          readImagePage(hextext, pageaddr, pagebuffersize, buffer1);

      if (!updi_run_tasks(UPDI_TASK_READ_FLASH, buffer2,
                          g_updi.config->flash_start + pageaddr,
                          pagebuffersize)) {
        return false;
      }

      for (uint16_t x = 0; x < pagebuffersize; x++) {
        if (buffer1[x] != buffer2[x]) {
          Serial.print(F("Verification error at address 0x"));
          Serial.print(pageaddr + x, HEX);
          Serial.print(F(": Should be 0x"));
          Serial.print(buffer1[x], HEX);
          Serial.print(F(" not 0x"));
          Serial.println(buffer2[x], HEX);
          Serial.println("----");
          for (uint16_t j = 0; j < pagebuffersize; j++) {
            Serial.print("0x");
            Serial.print(buffer1[j], HEX);
            if ((j % 16) == 15) {
              Serial.println();
            }
          }
          Serial.println(F("vs."));
          for (uint16_t j = 0; j < pagebuffersize; j++) {
            Serial.print(F("0x"));
            Serial.print(buffer2[j], HEX);
            if ((j % 16) == 15) {
              Serial.println();
            }
          }
          Serial.println(F("----"));
          return false;
        }
      }
      hextext = hextextpos;
      pageaddr += pagebuffersize;
    }
#endif
  } else {
    startProgramMode(FLASH_CLOCKSPEED); // start at 1MHz speed

    uint16_t len;
    byte b, cksum = 0;

    while (1) {
      uint16_t lineaddr;

      // read one line!
      char c = pgm_read_byte(hextext++);
      if (c == '\n' || c == '\r') {
        continue;
      }
      if (c != ':') {
        Serial.print(c);
        error(F(" No colon?"));
        break;
      }
      len = hexToByte(pgm_read_byte(hextext++));
      len = (len << 4) + hexToByte(pgm_read_byte(hextext++));
      cksum = len;

      b = hexToByte(pgm_read_byte(hextext++)); // record type
      b = (b << 4) + hexToByte(pgm_read_byte(hextext++));
      cksum += b;
      lineaddr = b;
      b = hexToByte(pgm_read_byte(hextext++)); // record type
      b = (b << 4) + hexToByte(pgm_read_byte(hextext++));
      cksum += b;
      lineaddr = (lineaddr << 8) + b;

      b = hexToByte(pgm_read_byte(hextext++)); // record type
      b = (b << 4) + hexToByte(pgm_read_byte(hextext++));
      cksum += b;

      // Serial.print("Record type "); Serial.println(b, HEX);
      if (b == 0x1) {
        // end record!
        break;
      }

      for (byte i = 0; i < len; i++) {
        // read 'n' bytes
        b = hexToByte(pgm_read_byte(hextext++));
        b = (b << 4) + hexToByte(pgm_read_byte(hextext++));
        cksum += b;

#if VERBOSE
        Serial.print("$");
        Serial.print(lineaddr, HEX);
        Serial.print(":0x");
        Serial.print(b, HEX);
        Serial.print(", ");
        if (lineaddr % 16 == 0)
          Serial.println();

#endif

        // verify this byte!
        byte reply;
        if (lineaddr % 2) { // for 'high' bytes
          reply = isp_transaction(0x28, lineaddr >> 9, lineaddr / 2, 0) & 0xFF;
        } else { // for 'low' bytes
          reply = isp_transaction(0x20, lineaddr >> 9, lineaddr / 2, 0) & 0xFF;
        }
        if (b != reply) {
          Serial.println();
          Serial.print(F("Verification error at address 0x"));
          Serial.print(lineaddr, HEX);
          Serial.print(F(" Should be 0x"));
          Serial.print(b, HEX);
          Serial.print(F(" not 0x"));
          Serial.println(reply, HEX);
          return false;
        }
        lineaddr++;
      }

      b = hexToByte(pgm_read_byte(hextext++)); // chxsum
      b = (b << 4) + hexToByte(pgm_read_byte(hextext++));
      cksum += b;
      if (cksum != 0) {
        Serial.print(cksum, HEX);
        error(F(" - bad checksum"));
      }
      if (pgm_read_byte(hextext++) != '\n') {
        error(F("No end of line"));
      }
    }

    endProgramMode();
  }
  return true;
}

/*******************************************************
 * Functions specific to ISP programming of an AVR
 */

// Send one byte to the page buffer on the chip
bool Adafruit_AVRProg::flashWord(uint8_t hilo, uint16_t addr, uint8_t data) {
#if VERBOSE
  Serial.print(data, HEX);
#endif
  if (isp_transaction(0x40 + 8 * hilo, addr >> 8 & 0xFF, addr & 0xFF, data) !=
      addr) {
    return false;
  }
#if VERBOSE
  Serial.print(" ");
#endif
  return true;
}

// Basically, write the pagebuff (with pagesize bytes in it) into page $pageaddr
bool Adafruit_AVRProg::flashPage(byte *pagebuff, uint16_t pageaddr,
                                 uint16_t pagesize) {
  Serial.print(F("Flashing page "));
  Serial.println(pageaddr, HEX);

#if VERBOSE
  for (uint16_t i = 0; i < pagesize; i++) {
    if (pagebuff[i] <= 0xF)
      Serial.print('0');
    Serial.print(pagebuff[i], HEX);
    Serial.print(" ");
    if (i % 16 == 15)
      Serial.println();
  }
#endif

  if (uart) {
#ifndef SUPPORT_UPDI
    return false;
#else
    // uint32_t t = millis();
    bool x =
        updi_run_tasks(UPDI_TASK_WRITE_FLASH, pagebuff, pageaddr, pagesize);
    // Serial.printf("Took %d millis\n", millis()-t);
    return x;
#endif
  } else {
    startProgramMode(FLASH_CLOCKSPEED);

    for (uint16_t i = 0; i < pagesize / 2; i++) {
      if (!flashWord(LOW, i, pagebuff[2 * i]))
        return false;
      if (!flashWord(HIGH, i, pagebuff[2 * i + 1]))
        return false;
    }

    // page addr is in bytes, but we need to convert to words (/2)
    // Serial.print("page addr "); Serial.println(pageaddr);
    pageaddr /= 2;

    uint16_t commitreply =
        isp_transaction(0x4C, (pageaddr >> 8) & 0xFF, pageaddr & 0xFF, 0);

    Serial.print(F("  Commit Page: 0x"));
    Serial.print(pageaddr, HEX);
    Serial.print(F(" -> 0x"));
    Serial.println(commitreply, HEX);
    if (commitreply != pageaddr)
      return false;

    busyWait();

    endProgramMode();
  }
  return true;
}

// Read one byte from EEPROM
int8_t Adafruit_AVRProg::readByteEEPROM(unsigned int addr) {
  return isp_transaction(0xA0, (addr >> 8) & 0x1F, addr & 0xFF, 0x00) & 0xFF;
}

// Simply polls the chip until it is not busy any more - for erasing and
// programming
void Adafruit_AVRProg::busyWait(void) {
  byte busybit;
  do {
    busybit = isp_transaction(0xF0, 0x0, 0x0, 0x0);
    // Serial.print(busybit, HEX);
  } while (busybit & 0x01);
}

/**************************************************************************/
/*!
    @brief  If you somehow get a chip that is expecting an external clock or
    crystal, we can generate a 8MHz square wave on ATmega328's digital 9 that
    can be connected to the crystal input to clock it. ONLY FOR AVR 'HOSTS'!
*/
/**************************************************************************/
void Adafruit_AVRProg::generateClock() {
#ifdef __AVR__
  Serial.println(F("Setting up 8MHz clock on pin 9"));
  pinMode(9, OUTPUT);
  // setup high freq PWM on pin 9 (timer 1)
  OCR1A = 0;
  ICR1 = 1; // 50% duty cycle -> 8 MHz
  // OC1A output, fast PWM
  TCCR1A = _BV(WGM11) | _BV(COM1A1);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10); // no clock prescale
#else
  error(F("Clock generation only supported on AVRs"));
#endif
}

/**************************************************************************/
/*!
 @brief  If your target chip will be running with internal RC oscillator,
 We can perform an calibration with the method described in AVR053.
 ONLY FOR AVR 'HOSTS' with a crystal oscillator, which is accurate.
 @return Calibrated value of OSCCAL, 0xFF if anything goes wrong.
 */
/**************************************************************************/
uint8_t Adafruit_AVRProg::internalRcCalibration() {
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) ||               \
    defined(__AVR_ATmega328__)
  Serial.println(F("Perform Internal RC Calibration"));

  unsigned char osscal_value = 0xFF;
  startProgramMode(FLASH_CLOCKSPEED);
  osscal_value = readByteEEPROM(0);
  endProgramMode();
  Serial.print(F("  Osccal is 0x"));
  Serial.print(osscal_value, HEX);
  Serial.println(F(" from EEPROM before calibration."));

  pinMode(_reset, OUTPUT);
  digitalWrite(_reset, LOW); // hold RST
  delay(50);
  pinMode(_miso, INPUT_PULLUP);
  pinMode(_mosi, INPUT);
  digitalWrite(_mosi, HIGH); // pull UP mosi
  digitalWrite(_miso, LOW);  // no pull up. Seems pull up will mess up with
                             // target
  digitalWrite(_sck, HIGH);  // pull UP sck, suppress noise
  // let MOSI act as OC2A, 16E6/(2*244)=32787

  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2 = 0;
  pinMode(_mosi, OUTPUT);
  TCCR2A = (0b01 << COM2A0) | (0b10 << WGM20); // TOGGLE MOSI on CTC
  TCCR2B = (0b001 << CS20);
  OCR2A = 243;

  delay(50);
  pinMode(_reset, INPUT); // release RST

  // waiting for calibration response
  unsigned char edge_count = 0;
  unsigned long millis_begin = millis();
  boolean cali_finished = false;
  boolean cali_last = HIGH;
  while (!cali_finished) {
    unsigned long millis_now = millis();
    if ((millis_now - millis_begin) > 600) {
      cali_finished = true;
    } else {
      boolean cali_input = digitalRead(MISO);
      if (cali_input != cali_last) {
        edge_count++;
        cali_last = cali_input;
        if (edge_count >= 8) {
          cali_finished = true;
        }
      }
    }
  }

  Serial.print(F("  "));
  Serial.print(edge_count);
  Serial.println(F(" edge received."));

  TCCR2A = 0;
  TCCR2B = 0;
  pinMode(_mosi, INPUT);
  digitalWrite(_mosi, LOW);
  digitalWrite(_miso, LOW);
  digitalWrite(_sck, LOW);

  if (edge_count >= 8) {
    Serial.println(F("  Chip Calibrated."));
  } else {
    Serial.println(F("Failed to calibrate chip"));
    return 0xFF;
  }
  startProgramMode(FLASH_CLOCKSPEED);
  osscal_value = readByteEEPROM(0);
  endProgramMode();
  Serial.print(F("  Osccal is 0x"));
  Serial.print(osscal_value, HEX);
  Serial.println(F(" from EEPROM after calibration."));

  return osscal_value;

#else
  error(F("Internal RC Calibration only supported on ATmega328 AVRs"));
  return 0xFF;
#endif
}

/**************************************************************************/
/*!
 @brief  Function to write a byte to certain address in flash without
 page erase. Useful for parameters.
 @param    addr Flash address you want to write to.
 @param    pagesize The flash-page size of this chip, in bytes. Check
 datasheet!
 @param    content The byte you want to write to.
 @return True if flashing worked out.
 */
/**************************************************************************/
bool Adafruit_AVRProg::writeByteToFlash(unsigned int addr, uint16_t pagesize,
                                        uint8_t content) {
  // calculate page number and offset.
  memset(pageBuffer, 0xFF, pagesize);
  uint16_t pageOffset = addr & (pagesize - 1);
  pageBuffer[pageOffset] = content;
  return flashPage(pageBuffer, addr, pagesize);
}

uint32_t Adafruit_AVRProg::isp_transaction(uint8_t a, uint8_t b, uint8_t c,
                                           uint8_t d) {
  uint8_t l, n, m, o;
  (void)o; // avoid unused var warning
  (void)m; // avoid unused var warning
  (void)n; // avoid unused var warning

#if VERBOSE > 1
  Serial.print("CMD [");
  Serial.print(a, HEX);
  Serial.print(" ");
  Serial.print(b, HEX);
  Serial.print(" ");
  Serial.print(c, HEX);
  Serial.print(" ");
  Serial.print(d, HEX);
  Serial.print("]");
#endif
  o = transfer(a);
  n = transfer(b);
  // if (n != a) error = -1;
  m = transfer(c);
  l = transfer(d);

#if VERBOSE > 1
  Serial.print(" [");
  Serial.print(o, HEX);
  Serial.print(" ");
  Serial.print(n, HEX);
  Serial.print(" ");
  Serial.print(m, HEX);
  Serial.print(" ");
  Serial.print(l, HEX);
  Serial.print("]\n");
#endif

  return ((m << 8) + l);
}

uint8_t Adafruit_AVRProg::transfer(uint8_t out) {
  if (spi) {
    return spi->transfer(out);
  } else if (_sck > 0 && _mosi > 0 && _miso > 0) {
    uint8_t in = 0;

    // Serial.print(" -> 0x"); Serial.print(out, HEX);
    for (int16_t i = 0x80; i > 0; i >>= 1) {
      digitalWrite(_sck, LOW);
      delayMicroseconds(spiBitDelay);
      // Serial.print("out: "); Serial.println(out, HEX);
      // Serial.print("i: "); Serial.println(i, HEX);
      if (out & i) {
        digitalWrite(_mosi, HIGH);
      } else {
        digitalWrite(_mosi, LOW);
      }

      in <<= 1;
      if (digitalRead(_miso))
        in |= 1;

      digitalWrite(_sck, HIGH);
      delayMicroseconds(spiBitDelay);
    }
    digitalWrite(_sck, LOW);
    // Make sure clock ends low

    // Serial.print(" <= 0x"); Serial.println(in, HEX);

    return in;
  }
  error(F("Neither hardware or software SPI modes selected"));
  return -1;
}

/*******************************************************
 * Low level support functions
 */

/*
 * hexToByte
 * Turn a Hex digit (0..9, A..F) into the equivalent binary value (0-16)
 */
byte Adafruit_AVRProg::hexToByte(byte h) {
  if (h >= '0' && h <= '9')
    return (h - '0');
  if (h >= 'A' && h <= 'F')
    return ((h - 'A') + 10);
  if (h >= 'a' && h <= 'f')
    return ((h - 'a') + 10);
  Serial.print("Read odd char 0x");
  Serial.print(h, HEX);
  Serial.println();

  error(F("Bad hex digit!"));
  return -1;
}

/**************************************************************************/
/*!
    @brief  Turn a pin on and off a few times; indicates life via LED
    @param  pin The arduino pin connected to the LED
    @param  times how many times to blink!
*/
/**************************************************************************/
void Adafruit_AVRProg::pulseLED(int pin, int times) {
  uint8_t PTIME = 30;
  pinMode(pin, OUTPUT);
  do {
    digitalWrite(pin, HIGH);
    delay(PTIME);
    digitalWrite(pin, LOW);
    delay(PTIME);
  } while (times--);
}

/**************************************************************************/
/*!
    @brief  Print an error, turn on the error LED and halt!
    @param  string What to print out before halting
*/
/**************************************************************************/
void Adafruit_AVRProg::error(const char *string) {
  Serial.println(string);
  if (errLED > 0) {
    pinMode(errLED, OUTPUT);
    digitalWrite(errLED, HIGH);
  }
  while (1) {
  }
}

/**************************************************************************/
/*!
    @brief  Print an error, turn on the error LED and halt!
    @param  string What to print out before halting
*/
/**************************************************************************/
void Adafruit_AVRProg::error(const __FlashStringHelper *string) {
  Serial.println(string);
  if (errLED > 0) {
    pinMode(errLED, OUTPUT);
    digitalWrite(errLED, HIGH);
  }
  while (1) {
  }
}
