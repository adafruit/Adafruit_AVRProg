/*
arduinoified https://gitlab.com/bradanlane/portaprog

MIT License

Copyright (c) 2020 bradanlane

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Adafruit_UPDIProg.h"
#include "Adafruit_AVRProg.h"

#ifdef SUPPORT_UPDI

DeviceIdentification g_updi_devices[] = {
    //	signature, short id, descriptive name, config
    {0x9123, "t202", "ATtiny202", AVR8X_TINY_2X},
    {0x9122, "t204", "ATtiny204", AVR8X_TINY_2X},
    {0x9121, "t212", "ATtiny212", AVR8X_TINY_2X},
    {0x9120, "t214", "ATtiny214", AVR8X_TINY_2X},
    {0x9223, "t402", "ATtiny402", AVR8X_TINY_4X},
    {0x9226, "t404", "ATtiny404", AVR8X_TINY_4X},
    {0x9225, "t406", "ATtiny406", AVR8X_TINY_4X},
    {0x9223, "t412", "ATtiny412", AVR8X_TINY_4X},
    {0x9222, "t414", "ATtiny414", AVR8X_TINY_4X},
    {0x9221, "t416", "ATtiny416", AVR8X_TINY_4X},
    {0x9220, "t417", "ATtiny417", AVR8X_TINY_4X},
    {0x9325, "t804", "ATtiny804", AVR8X_TINY_8X},
    {0x9324, "t806", "ATtiny806", AVR8X_TINY_8X},
    {0x9323, "t807", "ATtiny807", AVR8X_TINY_8X},
    {0x9322, "t814", "ATtiny814", AVR8X_TINY_8X},
    {0x9321, "t816", "ATtiny816", AVR8X_TINY_8X},
    {0x9320, "t817", "ATtiny817", AVR8X_TINY_8X},
    {0x9425, "t1604", "ATtiny1604", AVR8X_TINY_16X},
    {0x9424, "t1606", "ATtiny1606", AVR8X_TINY_16X},
    {0x9423, "t1607", "ATtiny1607", AVR8X_TINY_16X},
    {0x9422, "t1614", "ATtiny1614", AVR8X_TINY_16X},
    {0x9421, "t1616", "ATtiny1616", AVR8X_TINY_16X},
    {0x9420, "t1617", "ATtiny1617", AVR8X_TINY_16X},
    {0x9552, "m3208", "ATmega3208", AVR8X_MEGA_320},
    {0x9553, "m3209", "ATmega3209", AVR8X_MEGA_320},
    {0x9520, "t3214", "ATtiny3214", AVR8X_MEGA_321},
    {0x9521, "t3216", "ATtiny3216", AVR8X_MEGA_321},
    {0x9522, "t3217", "ATtiny3217", AVR8X_MEGA_321},
    {0x9650, "m4808", "ATmega4808", AVR8X_MEGA_480},
    {0x9651, "m4809", "ATmega4809", AVR8X_MEGA_480}};

DeviceConfiguration g_device_configs[] = {
    // flash address, flash size, flash page size, eeprom size, eeprom page size
    {0x8000, 2 * 1024, 64, 64, 32},    {0x8000, 4 * 1024, 64, 128, 32},
    {0x8000, 8 * 1024, 64, 128, 32},   {0x8000, 16 * 1024, 64, 256, 32},
    {0x4000, 32 * 1024, 128, 256, 64}, {0x8000, 32 * 1024, 128, 256, 64},
    {0x4000, 48 * 1024, 128, 256, 64}};
// TODO add support for larger chips (64, 128, and 256 KB)

uint8_t _updi_flash_page_buffer[AVR_PAGESIZE_MAX];

void Adafruit_AVRProg::updi_serial_init() {
  _updi_serial_retry_counter = 0;
  _updi_serial_retry_count = 0;

  // start slow
  uart->begin(min((uint32_t)_baudrate, (uint32_t)57600), SERIAL_8E2);
  uart->setTimeout(10);
  DEBUG_PHYSICAL("updi serial init set\n");

  _updi_serial_inited = true;
}

int Adafruit_AVRProg::updi_serial_read_wait(void) {
  uint32_t timeout = millis() + 250;
  int b = -1;
  _updi_serial_retry_counter++;
  _updi_serial_retry_count++;

  // try to wait for data
  while (millis() < timeout) {
    if (uart->available()) {
      b = uart->read();
      break;
    }
  }
  _updi_serial_retry_counter = 0;
  return b;
}

bool Adafruit_AVRProg::updi_serial_send(uint8_t *data, uint16_t size) {
  /*
    NOTE: since the TX and RX pins are tied together,
    everything we send gets echo'd and needs to be
    discarded QUICKLY.
  */
  bool good_echo = true;
  uint16_t count = 0;
  int b;

  // flush output and input
  uart->flush();
  while (uart->available()) {
    uart->read();
  }

  count = uart->write(data, size);
  if (count != size) {
    DEBUG_PHYSICAL("UPDISERIAL send count error %d != %d\n", count, size);
    return false;
  }

  yield();

  count = 0;
  for (uint16_t i = 0; i < size; i++) {
    b = updi_serial_read_wait(); // wait for data
    if (b != data[i]) {
      good_echo = false;
      DEBUG_PHYSICAL("\tsend[%d] %02x != %02x\n", i, data[i], b);
    } else {
      DEBUG_PHYSICAL("\tsend[%d] %02x == %02x\n", i, data[i], b);
    }
    count++;
  }
  if (count != size) {
    DEBUG_PHYSICAL("UPDISERIAL echo count error %d != %d\n", count, size);
    return false;
  }
  return good_echo; // was return true
}

bool Adafruit_AVRProg::updi_serial_send_receive(uint8_t *data, uint16_t size,
                                                uint8_t *buff, uint32_t len) {
  /*
          NOTE: since the TX and RX pins are tied together,
          everything we send gets echo'd and needs to be
          discarded QUICKLY.
  */
  bool timeout = false;
  uint32_t count = 0;
  int b;

  // uint32_t t = millis();

  if (updi_serial_send(data, size)) {
    for (uint32_t i = 0; i < len; i++) {
      b = updi_serial_read_wait(); // wait for data
      buff[count++] = b;
      if (b == -1)
        timeout = true;
      DEBUG_PHYSICAL("\treceive %d of %d =  %02x\n", i + 1, len, b);
    }
    if (count != len) {
      DEBUG_PHYSICAL("UPDISERIAL receive count error %d != %d\n", count, len);
      return false;
    }
    if (timeout) {
      DEBUG_PHYSICAL("UPDISERIAL timeout while reading data\n");
      return false;
    }
    return true;
  }
  return false;
}

bool Adafruit_AVRProg::updiIsConnected(bool silent) {
  updi_run_tasks(UPDI_TASK_GET_INFO, NULL);
  if (g_updi.initialized) {
    if (g_updi.unlocked) {
      if (!silent)
        Serial.printf("Detected %s\n", g_updi.device->longname);
      return true;
    }
  }
  if (!silent) {
    Serial.printf("No UPDI chip detected\n");
    Serial.printf("                             ");
    Serial.printf("Check the connection to the chip.\n");
    Serial.printf("                             ");
    Serial.printf("Attempt to cycle power to the chip.\n");
    Serial.printf("                             ");
    Serial.printf("If cycling power works, consider using VCC(sw).\n");
  }
  return false;
}

// Store a single byte value directly to a 16-bit address
bool Adafruit_AVRProg::updi_st(uint32_t address, uint8_t value) {
  uint8_t buf[4] = {UPDI_PHY_SYNC, UPDI_STS | UPDI_ADDRESS_16 | UPDI_DATA_8,
                    (uint8_t)(address & 0xFF),
                    (uint8_t)((address >> 8) & 0xFF)};
  uint8_t recv = 0;
  AVRDEBUG("ST(%02X) %02X %02X %02X %02X\n", value, buf[0], buf[1], buf[2],
           buf[3]);

  if (!updi_serial_send_receive(buf, 4, &recv, 1)) {
    DEBUG_PHYSICAL("ST error sending address");
    return false;
  } else {
    if (recv != UPDI_PHY_ACK) {
      DEBUG_PHYSICAL("error: st, no ACK from sent address\n");
      return false;
    }
  }

  buf[0] = value & 0xFF;

  if (!updi_serial_send_receive(buf, 1, &recv, 1)) {
    DEBUG_PHYSICAL("st error sending value\n");
    return false;
  } else {
    if (recv != UPDI_PHY_ACK) {
      DEBUG_PHYSICAL("error: st, no ACK after value sent\n");
      return false;
    }
  }

  return true;
}

bool Adafruit_AVRProg::udpi_stcs(uint8_t address, uint8_t value) {
  uint8_t buf[3] = {UPDI_PHY_SYNC, (uint8_t)(UPDI_STCS | (address & 0x0F)),
                    value};
  AVRDEBUG("STCS(%02X) %02X %02X %02X\n", value, buf[0], buf[1], buf[2]);

  return updi_serial_send(buf, 3);
}

// Load a single byte direct from a 16-bit address
uint8_t Adafruit_AVRProg::updi_ld(uint16_t address) {

  uint8_t buf[4] = {UPDI_PHY_SYNC, UPDI_LDS | UPDI_ADDRESS_16 | UPDI_DATA_8,
                    (uint8_t)(address & 0xFF),
                    (uint8_t)((address >> 8) & 0xFF)};
  uint8_t recv = 0;
  AVRDEBUG("LD %02X %02X %02X %02x\n", buf[0], buf[1], buf[2], buf[3]);

  if (!updi_serial_send_receive(buf, 4, &recv, 1)) {
    DEBUG_PHYSICAL("ld error\n");
    return 0;
  } else {
    return recv;
  }
}

// Load data from Control/Status space
uint8_t Adafruit_AVRProg::updi_ldcs(uint8_t address) {

  uint8_t buf[2] = {UPDI_PHY_SYNC, (uint8_t)(UPDI_LDCS | (address & 0x0F))};
  uint8_t recv = 0;
  AVRDEBUG("LDCS %02X %02X  ", buf[0], buf[1]);

  if (!updi_serial_send_receive(buf, 2, &recv, 1)) {
    DEBUG_PHYSICAL("updi_ldcs error\n");
    return 0;
  } else {
    AVRDEBUG("%02X\n", recv);
    return recv;
  }
}

// Loads a number of bytes from the pointer location with pointer post-increment
bool Adafruit_AVRProg::updi_ld_ptr_inc(uint8_t *buffer, uint16_t size) {
  uint8_t buf[2] = {UPDI_PHY_SYNC, UPDI_LD | UPDI_PTR_INC | UPDI_DATA_8};
  AVRDEBUG("LDPTRI(%d) %02X %02X\n", size, buf[0], buf[1]);

  if (!updi_serial_send_receive(buf, 2, buffer, size)) {
    DEBUG_PHYSICAL("in ld_ptr_inc(): error\n");
    return false;
  }

  return true;
}

// Set the pointer location
bool Adafruit_AVRProg::updi_st_ptr(uint32_t address) {
  uint8_t buf[4] = {UPDI_PHY_SYNC, UPDI_ST | UPDI_PTR_ADDRESS | UPDI_DATA_16,
                    (uint8_t)(address & 0xFF),
                    (uint8_t)((address >> 8) & 0xFF)};
  uint8_t recv = 0;
  AVRDEBUG("STPTR %02X %02X %02X %02X\n", buf[0], buf[1], buf[2], buf[3]);
  // AVRDEBUG("st ptr address 0x%04X\n", address);

  if (!updi_serial_send_receive(buf, 4, &recv, 1)) {
    DEBUG_PHYSICAL("st ptr error\n");
    return false;
  } else {
    if (recv != UPDI_PHY_ACK) {
      DEBUG_PHYSICAL("error: st_ptr no ACK\n");
      return false;
    }
  }

  return true;
}

// Store data to the pointer location with pointer post-increment
bool Adafruit_AVRProg::updi_st_ptr_inc(uint8_t *data, uint32_t size) {
  uint8_t buf[3] = {UPDI_PHY_SYNC, UPDI_ST | UPDI_PTR_INC | UPDI_DATA_8,
                    data[0]};
  uint8_t recv;
  AVRDEBUG("STPTRI(%d) %02X %02X %02X ", size, buf[0], buf[1], buf[2]);

  if (!updi_serial_send_receive(buf, 3, &recv, 1)) {
    DEBUG_PHYSICAL("error st_ptr_inc\n");
    return false;
  } else {
    if (recv != UPDI_PHY_ACK) {
      DEBUG_PHYSICAL("error: no ACK with st_ptr_inc");
      return false;
    }
  }

  for (uint32_t i = 1; i < size; i++) {
    recv = 0;
    AVRDEBUG(" %02X", data[i]);
    if (!updi_serial_send_receive(data + i, 1, &recv, 1)) {
      DEBUG_PHYSICAL("st_ptr_inc error\n");
      return false;
    } else {
      if (recv != UPDI_PHY_ACK) {
        DEBUG_PHYSICAL("error: no ACK with st_ptr_inc");
        return false;
      }
    }
  }
  AVRDEBUG("\n");
  return true;
}

// Store a 16-bit word value to the pointer location with pointer
// post-increment. Disable acks when we do this, to reduce latency.
void Adafruit_AVRProg::updi_st_ptr_inc16(uint8_t *data, uint32_t numwords) {
  uint8_t buf[2] = {UPDI_PHY_SYNC, UPDI_ST | UPDI_PTR_INC | UPDI_DATA_16};
  AVRDEBUG("STPTR16(%d) %02X %02X\n", numwords, buf[0], buf[1]);

  uint8_t ctrla_ackon = 1 << UPDI_CTRLA_IBDLY_BIT;
  uint8_t ctrla_ackoff = ctrla_ackon | (1 << UPDI_CTRLA_RSD_BIT);

  // disable acks
  udpi_stcs(UPDI_CS_CTRLA, ctrla_ackoff);

  updi_serial_send(buf, 2); // no response expected
  updi_serial_send(data, numwords << 1);

  // reenable acks
  udpi_stcs(UPDI_CS_CTRLA, ctrla_ackon);

  return;
}

// Store a value to the repeat counter
void Adafruit_AVRProg::updi_set_repeat(uint16_t repeats) {
  // DEBUG_PHYSICAL("set repeat %d\n", repeats);
  repeats -= 1;
  uint8_t buf[4] = {UPDI_PHY_SYNC, UPDI_REPEAT | UPDI_REPEAT_WORD,
                    (uint8_t)(repeats & 0xFF),
                    (uint8_t)((repeats >> 8) & 0xFF)};
  AVRDEBUG("REPT %02X %02X %02X %02X\n", buf[0], buf[1], buf[2], buf[3]);

  updi_serial_send(buf, 4);

  return;
}

bool Adafruit_AVRProg::updi_check(void) {
  DEBUG_PHYSICAL("updi_check()\n");
  if (updi_ldcs(UPDI_CS_STATUSA) != 0) {
    return true;
  }
  return false;
}

void Adafruit_AVRProg::updi_send_handshake(void) {
  uint8_t buf = UPDI_BREAK;
  updi_serial_send(&buf, 1);
  return;
}

bool Adafruit_AVRProg::updi_device_force_reset(void) {

  DEBUG_PHYSICAL("Sending BREAK BREAK\n");
  /*
          The BREAK character is used to reset the internal state of the UPDI to
     the default setting. This is useful if the UPDI enters an Error state due
     to a communication error or when the synchronization between the debugger
     and the UPDI is lost.

          To ensure that a BREAK is successfully received by the UPDI in all
     cases, the debugger must send two consecutive BREAK characters. The first
     BREAK will be detected if the UPDI is in Idle state and will not be
     detected if it is sent while the UPDI is receiving or transmitting (at a
     very low baud rate). However, this will cause a frame error for the
          reception (RX) or a contention error for the transmission (TX), and
     abort the ongoing operation. The UPDI will then detect the next BREAK
     successfully.

          The minimum BREAK is 6ms (a 20Mhz oscillator) and the worst-case is
     25ms.

          We could calculate the optimal BREAK using details about the chip ...
     or not
  */

  updi_serial_force_break();
  return true;
}

void Adafruit_AVRProg::updi_serial_force_break(void) {
  DEBUG_PHYSICAL("updi_serial_force_break()\n");

  // flush anything
  while (uart->available())
    uart->read();

  DEBUG_PHYSICAL("updi_serial baud 110\n");
  uart->begin(110);
  delay(50);

  DEBUG_PHYSICAL("updi_serial BREAK 1\n");
  uart->write((byte)0);
  // flush anything
  while (uart->available())
    uart->read();

  delay(12);

  DEBUG_PHYSICAL("updi_serial BREAK 2\n");
  uart->write((byte)0);
  while (uart->available())
    uart->read();

  DEBUG_PHYSICAL("updi_serial baud 115200\n");
  uart->begin(115200);
}

bool Adafruit_AVRProg::updi_init(bool force) {
  if (force && (_power >= 0)) {
    pinMode(_power, OUTPUT);
    digitalWrite(_power, _invertpower);
    delay(10);
    digitalWrite(_power, !_invertpower);
    delay(10);
  }
  updi_serial_init();
  updi_send_handshake();
  delay(3);

  udpi_stcs(UPDI_CS_CTRLB, 1 << UPDI_CTRLB_CCDETDIS_BIT);
  udpi_stcs(UPDI_CS_CTRLA, 1 << UPDI_CTRLA_IBDLY_BIT);
  if (_baudrate > 230000) {
    udpi_stcs(UPDI_ASI_CTRLA, 0x1); // set 16mhz for higher baudrate!
  }
  uart->begin(_baudrate, SERIAL_8E2);

  return updi_check();
}

// run the updi process based on the 'command bits' provided
bool Adafruit_AVRProg::updi_run_tasks(uint16_t tasks, uint8_t *data,
                                      uint32_t address, uint32_t size) {
  long unsigned int start = millis();
  // int32_t datasize = 0;
  bool success = true;

  if (!(tasks & (UPDI_TASKS))) {
    DEBUG_TASK("No UPDI tasks specified\n");
    return true;
  }

  uint8_t saved_fuses[AVR_NUM_FUSES];
  if (tasks & UPDI_TASK_WRITE_FUSES) {
    // we need to preserve the fuses through the device setup process
    // g_updi.fuses contains the new fuse values
    for (uint8_t i = 0; i < AVR_NUM_FUSES; i++)
      saved_fuses[i] = g_updi.fuses[i];
  }
  DEBUG_TASK("Checking for UPDI chip\n");

  updi_init(true);

  do { // we use a do {} while (0); so we have easy branch control

    if (!updi_check()) {
      DEBUG_TASK("UPDI not initialised\n");

      if (!updi_device_force_reset()) {
        DEBUG_TASK("double BREAK reset failed\n");
        success = false;
        break;
      }
      updi_init(false); // re-init the UPDI interface

      if (!updi_check()) {
        DEBUG_PHYSICAL("Cannot initialise UPDI, aborting.\n");
        // TODO find out why these are not already correct
        g_updi.initialized = false;
        g_updi.unlocked = false;
        success = false;
        break;
      } else {
        DEBUG_PHYSICAL("UPDI INITIALISED\n");
        g_updi.initialized = true;
      }
    } else {
      DEBUG_PHYSICAL("UPDI ALREADY INITIALISED\n");
      g_updi.initialized = true;
    }

    // enter progmode & unlock if needed && write flash / erase set since
    // unlocking erases
    if (!updi_enter_progmode()) {
      DEBUG_TASK("Couldnt enter progmode\n");

      if ((tasks & UPDI_TASK_ERASE) || (tasks & UPDI_TASK_WRITE_FLASH)) {
        DEBUG_TASK("erasing and unlocking device\n");

        updi_unlock_device();

        if (updi_is_prog_mode()) {
          DEBUG_PHYSICAL("IN PROG MODE HARD\n");
          g_updi.unlocked = true;
        } else {
          Serial.printf("Could not enter programming mode, aborting.\n");
          g_updi.unlocked = false;
          success = false;
          break;
        }

      } else {
        // Serial.printf("Need to erase device to unlock progmode. Need process
        // args UPDI_TASK_ERASE or UPDI_TASK_WRITE_FLASH set\n");
        Serial.printf("UPDI chip is locked\n");
        g_updi.unlocked = false;
        success = false;
        break;
      }
    } else {
      DEBUG_PHYSICAL("IN PROG MODE EASY\n");
      g_updi.unlocked = true;
    }

    if (!updi_get_device_info()) {
      DEBUG_TASK("Unable to get chip information - may not be UPDI capable.");
      success = false;
      break;
    };

    // TODO find out where we reset these because it was a mistake
    g_updi.initialized = true;
    g_updi.unlocked = true;

    // do requested actions
    if (tasks & UPDI_TASK_GET_INFO) {
      DEBUG("\nGETTING / GOT DEVICE INFO\n");
      // _updi_get_device_info(); // we dont actually need to do anything as the
      // info was previously fetched for all operations
    }

    // save fuses into updi array, also read before writing the fuses
    // to determine which ones actually need to be changed
    // New fuse values have been saved in saved_fuses on function entry
    if ((tasks & UPDI_TASK_READ_FUSES) || (tasks & UPDI_TASK_WRITE_FUSES)) {
      Serial.printf("Reading fuses\n");

      for (uint8_t i = 0; i < AVR_NUM_FUSES; i++) {
        uint8_t value = updi_read_fuse(i);
        g_updi.fuses[i] = value;
      }
    }

    // write fuses from updi array
    if (tasks & UPDI_TASK_WRITE_FUSES) {
      Serial.printf("Writing fuses\n");

      // Only write fuses whose value needs to be changed
      // Especially important for lockbits, as writing any value
      // to lockbits (including 'valid key' 0xC5) seems to lock the device
      for (uint8_t i = 0; i < AVR_NUM_FUSES; i++) {
        if (saved_fuses[i] != g_updi.fuses[i]) {
          DEBUG_FUSES("Write fuse %d: 0x%02X -> 0x%02X\n", i, g_updi.fuses[i],
                      saved_fuses[i]);
          updi_write_fuse(i, saved_fuses[i]);
        } else {
          DEBUG_FUSES("Skip fuse %d: 0x%02X -> 0x%02X\n", i, g_updi.fuses[i],
                      saved_fuses[i]);
        }
      }
    }

    // Erase flash
    if (tasks & UPDI_TASK_ERASE) {
      Serial.printf("Erasing flash\n");

      if (!updi_erase_chip()) {
        Serial.printf("Chip erase failed\n");
        success = false;
        break;
      }
    }

    // Write flash from hex file
    if (tasks & UPDI_TASK_WRITE_FLASH) {
      Serial.printf("delta: %lu millis\n", millis() - start);
      int16_t flashpagesize = g_updi.config->flash_pagesize;
      int32_t remainingsize = size;

      DEBUG("Writing %d bytes starting at %04X\n", size, address);
      if (data == NULL) {
        Serial.println(F("Data pointer null"));
        success = false;
        break;
      }

      while (remainingsize > 0) {
        bool page_blank = true;
        for (uint32_t p = 0; p < flashpagesize; p++) {
          if (data[p] != 0xFF) {
            page_blank = false;
            break;
          }
        }
        if (!page_blank) {
          if (!updi_write_nvm(address, data, flashpagesize,
                              UPDI_NVMCTRL_CTRLA_updi_write_PAGE, true,
                              false)) {
            // Serial.println("Writing flash failed");
            success = false;
            break;
          } else {
            // Serial.println("Flash written\n");
          }
        }
        remainingsize -= flashpagesize;
        address += flashpagesize;
        data += flashpagesize;
      }
    }

    // save flash into updi array
    if (tasks & UPDI_TASK_READ_FLASH) {
      DEBUG("Reading %d bytes starting at %04X\n", size, address);
      if (data == NULL) {
        Serial.println(F("Data pointer null"));
        success = false;
        break;
      }

      if (!updi_read_page(address, size, data)) {
        Serial.println("Reading flash failed");
        success = false;
        break;
      } else {
        // Serial.println("Reading flash OK");
      }
    }

    /*
            //Write eeprom from hex file
            if (tasks & UPDI_TASK_WRITE_EEPROM) {
                    MESSAGE("Writing eeprom\n");
                    data->rewind();
                    datasize = data->available();
                    //load hex, determine size
                    if (!datasize) {
                            MESSAGE("No data to flash\n");
                            break;
                    }

                    MESSAGE("Writing eeprom %d bytes: \n", datasize);

                    if (!_updi_write_address_space(AVR_EEPROM_ADDR,
       g_updi.config->eeprom_pagesize, datasize, data, false)) {
                            MESSAGE("Writing eeprom failed\n");
                            break;
                    } else {
                            MESSAGE("eeprom written\n");
                    }
            }

            //save eeprom into updi array
            if (tasks & UPDI_TASK_READ_EEPROM) {
                    MESSAGE("Reading eeprom\n");
                    //hexfInit();
                    DEBUG_PHYSICAL("Reading %d bytes starting at %04X\n",
       g_updi.config->eeprom_size, AVR_EEPROM_ADDR);

                    // read eeprom and store in memory buffer
                    data->rewind();
                    if (!_updi_read_address_space(AVR_EEPROM_ADDR,
       g_updi.config->eeprom_pagesize, g_updi.config->eeprom_size, data, true))
       { MESSAGE("Reading eeprom failed\n"); break; } else { data->rewind();
                            //data->resize(g_updi.config->flash_size,
       g_updi.config->flash_pagesize); datasize = data->available();
                            MESSAGE("Read %d bytes: \n", datasize);
                    }
            }
    */
  } while (0);

  // leave progmode
  updi_leave_progmode();

  // Tidy up
  updi_term();

  DEBUG_TASK("TASK RUN TIME: %ld ms\n", millis() - start);
  DEBUG_TASK("UPDI Serial retry counter: %d\n", _updi_serial_retry_count);

  DEBUG_TASK("UPDI tasks finished\n");
  return success;
}

void Adafruit_AVRProg::updi_term() {
  updi_serial_term();
  delay(5);
}

void Adafruit_AVRProg::updi_serial_term() {
  DEBUG_PHYSICAL("updi serial term begin\n");
  uart->flush();

  delay(10);
  DEBUG_PHYSICAL("updi serial term flushed\n");
  delay(10);
  uart->end();
  DEBUG_PHYSICAL("updi serial term closed\n");
  _updi_serial_inited = false;
}

void Adafruit_AVRProg::updi_apply_reset() {
  DEBUG_PHYSICAL("Applying reset\n");
  udpi_stcs(UPDI_ASI_RESET_REQ, UPDI_RESET_REQ_VALUE);

  if (!(updi_ldcs(UPDI_ASI_SYS_STATUS) & (1 << UPDI_ASI_SYS_STATUS_RSTSYS))) {
    DEBUG_PHYSICAL("error applying reset\n");
    return;
  }

  delay(5);
  DEBUG_PHYSICAL("Releasing reset\n");
  udpi_stcs(UPDI_ASI_RESET_REQ, 0x00);
  uint8_t retries = (255 - 10); // at most 10 retries

  while (retries++) {
    uint8_t b;
    if (!((b = updi_ldcs(UPDI_ASI_SYS_STATUS)) &
          (1 << UPDI_ASI_SYS_STATUS_RSTSYS)))
      break;
    // DEBUG_VERBOSE("Wait for release %03d ( %02X != %02X)\n", retries, b, (1
    // << UPDI_ASI_SYS_STATUS_RSTSYS));
  }
  if (!retries) {
    // if our retry counter rolled over, then we failed to reset
    DEBUG_PHYSICAL("Error releasing reset\n");
  }
}

// Waits for the device to be unlocked. All devices boot up as locked until
// proven otherwise
bool Adafruit_AVRProg::updi_wait_unlocked(uint32_t timeout) {
  unsigned long end = millis() + timeout;

  while (millis() < end) {
    if (!(updi_ldcs(UPDI_ASI_SYS_STATUS) &
          (1 << UPDI_ASI_SYS_STATUS_LOCKSTATUS))) {
      g_updi.unlocked = true;
      return true;
    }
  }

  DEBUG_VERBOSE("TIMEOUT WAITING FOR DEVICE TO UNLOCK\n");
  g_updi.unlocked = false;

  return false;
}

bool Adafruit_AVRProg::updi_is_prog_mode() {
  if (updi_ldcs(UPDI_ASI_SYS_STATUS) & (1 << UPDI_ASI_SYS_STATUS_NVMPROG)) {
    return true;
  } else {
    return false;
  }
}

// Inserts the NVMProg key and updi_checks that its accepted
bool Adafruit_AVRProg::updi_progmode_key() {
  updi_write_key(UPDI_KEY_64, (uint8_t *)UPDI_KEY_NVM);

  uint8_t key_status = updi_ldcs(UPDI_ASI_KEY_STATUS);

  if (!(key_status & (1 << UPDI_ASI_KEY_STATUS_NVMPROG))) {
    return false;
  }

  return true;
}

bool Adafruit_AVRProg::updi_enter_progmode() {

  // Enter NVMProg key
  if (!updi_is_prog_mode()) {
    if (!updi_progmode_key()) {
      return false;
    }
  }

  updi_apply_reset();

  // Wait for unlock
  if (!updi_wait_unlocked(100)) {
    DEBUG_VERBOSE("FAILED TO ENTER NVM PROGRAMMING MODE, DEVICE IS LOCKED\n");
    return false;
  }

  // updi_check for NVMPROG flag
  if (!updi_is_prog_mode()) {
    DEBUG_VERBOSE("STILL NOT IN PROG MODE\n");
    return false;
  }
  g_updi.unlocked = true;
  return true;
}

// Disables UPDI which releases any keys enabled
void Adafruit_AVRProg::updi_leave_progmode() {
  DEBUG_VERBOSE("leaving progmode...\n");
  updi_apply_reset();
  udpi_stcs(UPDI_CS_CTRLB,
            (1 << UPDI_CTRLB_UPDIDIS_BIT) | (1 << UPDI_CTRLB_CCDETDIS_BIT));

  return;
}

/**************************************************************************/
/*!
    @brief  Perform a quick UPDI unlock/erase cycle (for post-fuse writes)
    @returns UPDI command success status
*/
/**************************************************************************/
bool Adafruit_AVRProg::UPDIunlock(void) {
  updi_init(true);

  if (!updi_check()) {
    DEBUG_TASK("UPDI not initialised\n");

    if (!updi_device_force_reset()) {
      DEBUG_TASK("double BREAK reset failed\n");
      return false;
    }
    updi_init(false); // re-init the UPDI interface

    if (!updi_check()) {
      DEBUG_PHYSICAL("Cannot initialise UPDI, aborting.\n");
      // TODO find out why these are not already correct
      g_updi.initialized = false;
      g_updi.unlocked = false;
      return false;
    } else {
      DEBUG_PHYSICAL("UPDI INITIALISED\n");
      g_updi.initialized = true;
    }
  } else {
    DEBUG_PHYSICAL("UPDI ALREADY INITIALISED\n");
    g_updi.initialized = true;
  }

  if (updi_ldcs(UPDI_ASI_SYS_STATUS) & (1 << UPDI_ASI_SYS_STATUS_LOCKSTATUS)) {
    Serial.println("We are in fact locked");
  }

  // enter key
  updi_write_key(UPDI_KEY_64, (uint8_t *)UPDI_KEY_CHIPERASE);

  // updi_check key status
  uint8_t key_status = updi_ldcs(UPDI_ASI_KEY_STATUS);
  if (!(key_status & (1 << UPDI_ASI_KEY_STATUS_CHIPERASE))) {
    DEBUG_VERBOSE("Unlock error: key not accepted\n");
    return false;
  }
  Serial.println("Unlock key inserted");

  updi_apply_reset();

  updi_wait_unlocked(500);

  return true;
}

// Unlock and erase
bool Adafruit_AVRProg::updi_unlock_device() {
  DEBUG_VERBOSE("UNLOCKING AND ERASING\n");

  // enter key
  updi_write_key(UPDI_KEY_64, (uint8_t *)UPDI_KEY_CHIPERASE);

  // updi_check key status
  uint8_t key_status = updi_ldcs(UPDI_ASI_KEY_STATUS);

  if (!(key_status & (1 << UPDI_ASI_KEY_STATUS_CHIPERASE))) {
    DEBUG_VERBOSE("Unlock error: key not accepted\n");
    return false;
  }

  // Insert NVMProg key as well
  // In case of CRC being enabled, the device must be left in programming mode
  // after the erase to allow the CRC to be disabled (or flash reprogrammed)
  updi_progmode_key();

  updi_apply_reset();

  // wait for unlock
  if (!updi_wait_unlocked(500)) {
    DEBUG_VERBOSE("Failed to chip erase using key\n");
    return false;
  }

  DEBUG_VERBOSE("UNLOCKED DEVICE\n");

  return true;
}

// Get device info
bool Adafruit_AVRProg::updi_get_device_info() {
  DEBUG_VERBOSE("_updi_get_device_info()\n");
  uint8_t buf[2] = {UPDI_PHY_SYNC, UPDI_KEY | UPDI_KEY_SIB | UPDI_SIB_16BYTES};
  uint8_t recv[16]; // buffer for received data; reused multiple times
  AVRDEBUG("SIB %02X %02X\n", buf[0], buf[1]);

  updi_chip_data_init_info(0x00, NULL, true); // clear out any prior data

  if (!updi_serial_send_receive(buf, 2, recv, 16)) {
    DEBUG_VERBOSE("device info recv error\n");
  } else {
    for (uint8_t i = 0; i < 7; i++)
      g_updi.details.family[i] = recv[i];
    for (uint8_t i = 8; i < 11; i++)
      g_updi.details.nvm_version[i - 8] = recv[i];
    for (uint8_t i = 11; i < 14; i++)
      g_updi.details.ocd_version[i - 11] = recv[i];
    g_updi.details.dbg_osc_freq = recv[15];

    DEBUG_VERBOSE("Chip Family: %s\n", g_updi.details.family);

    // if we are in program mode we can get additional details
    // the SIGROW address starts with 3 bytes which hold the chip type (always
    // starts with 0x1E the next 10 bytes are the chip unique id
    if (updi_is_prog_mode()) {
      updi_read_data(AVR_SIG_ADDRESS, recv, 3 + 10);

      for (int i = 0; i < 3; i++)
        g_updi.details.signature_bytes[i] = recv[i];

      for (int i = 0; i < 10; i++)
        g_updi.details.uid[i] = recv[3 + i];

      // our data lookup uses the 16 bit value of the chip type (ignoring the
      // leading 0x1E)
      uint16_t signature = ((recv[1] << 8) + recv[2]);
      DEBUG_VERBOSE("Chip signature: %04X\n", signature);
      updi_chip_data_init_info(signature, NULL, false);

      updi_read_data(AVR_SYSCFG_ADDRESS, recv, 1);
      g_updi.details.dev_rev = recv[0];

      g_updi.details.pdi_rev = (updi_ldcs(UPDI_CS_STATUSA) >> 4);

      // mostly for geeky interest, we also grab the oscillator error correction
      // data
      updi_read_data(AVR_SIG_ADDRESS + 0x20, recv, 6);
      // the error data consists of 2 bytes of temperature sensors, 2 bytes of
      // oscillator at 3v and 2 bytes of oscillator at 5 bytes
      g_updi.details.error_16v3 = recv[2];
      g_updi.details.error_16v5 = recv[3];
      g_updi.details.error_20v3 = recv[4];
      g_updi.details.error_20v5 = recv[5];

      g_updi.initialized = true;
    }
  }
  return g_updi.initialized;
}

// Does a chip erase using the NVM controller Note that on locked devices this
// it not possible and the ERASE KEY has to be used instead
bool Adafruit_AVRProg::updi_erase_chip() {
  DEBUG_VERBOSE("ERASING CHIP...\n");

  // Wait until NVM CTRL is ready to erase
  if (!updi_wait_flash_ready()) {
    DEBUG_VERBOSE("in updi_erase_chip() error: timeout waiting for flash ready "
                  "before erase\n");
    return false;
  }

  // Erase
  if (!updi_execute_nvm_command(UPDI_NVMCTRL_CTRLA_updi_erase_chip)) {
    DEBUG_VERBOSE(
        "in updi_erase_chip() error: execute_nvm_command() failed()\n");
    return false;
  }

  // Wait to finish
  if (!updi_wait_flash_ready()) {
    DEBUG_VERBOSE("in updi_erase_chip() error: timeout waiting for flash ready "
                  "after erase\n");
    return false;
  }

  DEBUG_VERBOSE("ERASED\n");

  return true;
}

// Reads one fuse value
uint8_t Adafruit_AVRProg::updi_read_fuse(uint8_t fuse) {
  DEBUG_VERBOSE("updi_read_fuse(%d)\n", fuse);

  if (!updi_is_prog_mode()) {
    DEBUG_VERBOSE("in updi_read_fuse() error: not in prog mode\n");
    return 0;
  }

  return updi_ld(AVR_FUSE_BASE + fuse);
}

// Writes one fuse value
bool Adafruit_AVRProg::updi_write_fuse(uint8_t fuse, uint8_t value) {
  uint8_t data;

  DEBUG_FUSES("updi_write_fuse: fuse %d value 0x%02X\n", fuse, value);
  // Serial.print("Write fuse "); Serial.println(fuse, DEC);

  if (!updi_is_prog_mode()) {
    DEBUG_VERBOSE("in updi_write_fuse() error: not in prog mode\n");
    return false;
  }

  if (!updi_wait_flash_ready()) {
    DEBUG_VERBOSE("in updi_write_fuse() error: cant wait flash ready\n");
    return false;
  }

  data = (AVR_FUSE_BASE + fuse) & 0xff;
  if (!updi_write_data(AVR_NVM_ADDRESS + UPDI_NVMCTRL_ADDRL, &data, 1)) {
    DEBUG_VERBOSE("in updi_write_fuse() error: write data fail\n");
    return false;
  }

  data = (AVR_FUSE_BASE + fuse) >> 8;
  if (!updi_write_data(AVR_NVM_ADDRESS + UPDI_NVMCTRL_ADDRH, &data, 1)) {
    DEBUG_VERBOSE("in updi_write_fuse() error: write data fail\n");
    return false;
  }

  if (!updi_write_data(AVR_NVM_ADDRESS + UPDI_NVMCTRL_DATAL, &value, 1)) {
    DEBUG_VERBOSE("in updi_write_fuse() error: write data fail\n");
    return false;
  }

  data = UPDI_NVMCTRL_CTRLA_UPDI_WRITE_FUSE;
  if (!updi_write_data(AVR_NVM_ADDRESS + UPDI_NVMCTRL_CTRLA, &data, 1)) {
    DEBUG_VERBOSE("in updi_write_fuse() error: write data fail\n");
    return false;
  }

  return true;
}

bool Adafruit_AVRProg::updi_read_page(uint16_t address, uint16_t bufsize,
                                      uint8_t *bufdata) {
  if (!updi_is_prog_mode()) {
    DEBUG_VERBOSE("dump_flash() error: not in prog mode\n");
    return false;
  }
  if (bufsize > AVR_PAGESIZE_MAX) {
    Serial.printf("Chip buffer %d bytes exceeds %d byte maximum\n", bufsize,
                  AVR_PAGESIZE_MAX);
    return false;
  }
  // address = address - (address % pagesize); // round down to a page address
  Serial.printf("Reading %d bytes from 0x%x\n", bufsize,
                address); // deliberately no LF; the start + progress + finish
                          // messages are all combined

  uint8_t err_count = 0;
  bool success = false;
  while (!success) {
    success = updi_read_data(address, bufdata, bufsize);

    if (!success) {
      err_count++;
      DEBUG_VERBOSE("read words error at 0x%04X\n", address);
      if (err_count > 4) { // too many consecutive errors
        Serial.printf(" Failed at %d bytes\n", address);
        return false;
      }
      continue;
    }
    err_count = 0;

    for (uint32_t j = 0; j < bufsize; j++) {
      AVRDEBUG("%02X ", bufdata[j]);
    }
    AVRDEBUG("\n");
  }

  // Serial.println("\nSuccess");
  return true;
}

bool Adafruit_AVRProg::updi_write_page(uint16_t address, uint16_t pagesize,
                                       uint8_t *pagedata) {
  // Serial.println("Writing a page");
  uint32_t t = millis();

  if (!updi_is_prog_mode()) {
    DEBUG_VERBOSE("in updi_write_flash error: not in prog mode\n");
    return false;
  }

  // Serial.println(F("Is in prog mode"));
  if (pagesize > AVR_PAGESIZE_MAX) {
    Serial.printf("Chip pagesize %d bytes exceeds %d byte maximum\n", pagesize,
                  AVR_PAGESIZE_MAX);
    return false;
  }
  address = address - (address % pagesize); // round down to a page address
  Serial.printf("Writing %d bytes to 0x%x\n", pagesize,
                address); // deliberately no LF; the start + progress + finish
                          // messages are all combined

  Serial.printf("before write: %lu millis\n", millis() - t);
  t = millis();
  if (!updi_write_nvm(address, pagedata, pagesize,
                      UPDI_NVMCTRL_CTRLA_updi_write_PAGE, true)) {
    Serial.printf(" Failed\n");
    return false;
  }
  Serial.printf("updi_write_nvm: %lu millis\n", millis() - t);

  // Serial.printf("Finished\n");
  return true;
}

// Write a key
void Adafruit_AVRProg::updi_write_key(uint8_t size, uint8_t *key) {
  uint16_t len = 8 << size;
  uint8_t buf[2] = {UPDI_PHY_SYNC, (uint8_t)(UPDI_KEY | UPDI_KEY_KEY | size)};
  uint8_t key_reversed[len];
  AVRDEBUG("KEY %02X %02X  ", buf[0], buf[1]);

  for (uint16_t i = 0; i < len; i++) {
    AVRDEBUG("%c", key[i]);
    key_reversed[i] = key[len - 1 - i];
  }
  AVRDEBUG("\n");

  updi_serial_send(buf, 2);
  updi_serial_send(key_reversed, 8 << size);

  return;
}

bool Adafruit_AVRProg::updi_chip_data_init_info(uint16_t sig, char *shortname,
                                                bool format) {
  // one of (dev, sig, name) must be valid

  // optionally initialize everything to zeros
  if (format)
    memset(&g_updi, 0, sizeof(UPDI));

  // get the device identification using any one of: list index, signature, or
  // name
  g_updi.device = updi_chip_lookup(sig, shortname);

  if (!g_updi.device)
    return false;

  g_updi.config = &g_device_configs[g_updi.device->config];

  return true;
}

// provide lookup using signature or shortname
DeviceIdentification *Adafruit_AVRProg::updi_chip_lookup(uint16_t sig,
                                                         char *name) {
  uint8_t max = sizeof(g_updi_devices) / sizeof(DeviceIdentification);

  // search for the correct device identification using any one of: list index,
  // signature, or name the priority for matching is: signature, name, then
  // index because an index of 0 is valid but a signature of 0 is not so we can
  // differentiate missing parameters
  for (uint8_t i = 0; i < max; i++) {
    if (sig) {
      if (g_updi_devices[i].signature == sig) {
        DEBUG_VERBOSE("Found Chip %s\n", g_updi_devices[i].longname);
        return &g_updi_devices[i];
      }
    } else if (name && name[0]) {
      // as a courtesy, we check for both the shortname and the longname; eg:
      // m4809 and ATmega4809
      if (strcasecmp(g_updi_devices[i].shortname, name) == 0) {
        return &g_updi_devices[i];
      }
      if (strcasecmp(g_updi_devices[i].longname, name) == 0) {
        return &g_updi_devices[i];
      } else {
        // if we were not given a signature or a shortname, then we have no
        // chance of finding the device
        return NULL;
      }
    }
  }
  return NULL;
}

// Reads a number of bytes of data from UPDI
bool Adafruit_AVRProg::updi_read_data(uint32_t address, uint8_t *buf,
                                      uint32_t size) {

  // Range updi_check
  if (size > UPDI_MAX_REPEAT_SIZE + 1) {
    DEBUG_VERBOSE("read_data error: cant read that many bytes at once\n");
    return false;
  }

  if (!updi_st_ptr(address)) { // Store address pointer
    DEBUG_VERBOSE("read_data() error: st_ptr()\n");
    return false;
  }
  if (size > 1)
    updi_set_repeat(size); // Set repeat

  if (!updi_ld_ptr_inc(buf, size)) {
    DEBUG_VERBOSE("updi_read_data(): ld_ptr_inc error\n");
    return false;
  }

  return true;
}

// Writes a number of bytes to memory
bool Adafruit_AVRProg::updi_write_data(uint32_t address, uint8_t *data,
                                       uint32_t len) {
  if (len == 1) {
    if (!updi_st(address, data[0])) {
      DEBUG_VERBOSE("in _updi_write_data() error: st ret false\n");
      return false;
    }
  } else if (len == 2) {
    if (!updi_st(address, data[0])) {
      DEBUG_VERBOSE("in _updi_write_data() error: st ret false\n");
      return false;
    }
    if (!updi_st(address + 1, data[1])) {
      DEBUG_VERBOSE("in _updi_write_data() error: st ret false\n");
      return false;
    }
  } else {
    // Range updi_check
    if (len > UPDI_MAX_REPEAT_SIZE + 1) {
      DEBUG_VERBOSE("in _updi_write_data() error: invalid length\n");
      return false;
    }

    // store address
    if (!updi_st_ptr(address)) {
      DEBUG_VERBOSE("in _updi_write_data() error: couldnt st_ptr(address)\n");
      return false;
    }

    // set up repeat
    updi_set_repeat(len);
    if (!updi_st_ptr_inc(data, len)) {
      DEBUG_VERBOSE(
          "in _updi_write_data() error: couldnt st_ptr_inc() error\n");
      return false;
    }
  }

  return true;
}

// Writes a number of words to memory
bool Adafruit_AVRProg::updi_write_data_words(uint32_t address, uint8_t *data,
                                             uint32_t numwords) {
  if (numwords == 1) {
    return updi_write_data(address, data, 2);
  } else {
    // Range updi_check
    if (numwords > ((UPDI_MAX_REPEAT_SIZE >> 1) + 1)) {
      DEBUG_VERBOSE("in _updi_write_data_words error: invalid length\n");
      return false;
    }

    // Store address
    if (!updi_st_ptr(address)) {
      DEBUG_VERBOSE("in _updi_write_data_words error: st_ptr() error\n");
      return false;
    }

    // Set up repeat
    updi_set_repeat(numwords);
    updi_st_ptr_inc16(data, numwords);
  }

  return true;
}

// Execute NVM command
bool Adafruit_AVRProg::updi_execute_nvm_command(uint8_t command) {
  if (!updi_st(AVR_NVM_ADDRESS + UPDI_NVMCTRL_CTRLA, command)) {
    DEBUG_VERBOSE("in execute_nvm_command() error: st() false return\n");
    return false;
  }
  return true;
}

// Waits for the NVM controller to be ready
bool Adafruit_AVRProg::updi_wait_flash_ready() {
  uint32_t end = millis() + 10000;

  while (millis() < end) {
    uint8_t status = updi_ld(AVR_NVM_ADDRESS + UPDI_NVMCTRL_STATUS);
    if (status & (1 << UPDI_NVM_STATUS_UPDI_WRITE_ERROR)) {
      DEBUG_VERBOSE("in wait_flash_ready() error: nvm error\n");
      return false;
    }

    if (!(status & ((1 << UPDI_NVM_STATUS_EEPROM_BUSY) |
                    (1 << UPDI_NVM_STATUS_FLASH_BUSY)))) {
      return true;
    }
  }

  DEBUG_VERBOSE("in wait_flash_ready() error: wait flash ready timed out\n");
  return false;
}

// Writes a page of data to NVM. By default the PAGE_WRITE command is used,
// which requires that the page is already erased. By default word access is
// used (flash)
bool Adafruit_AVRProg::updi_write_nvm(uint32_t address, uint8_t *data,
                                      uint32_t len, uint8_t command,
                                      bool use_word_acess, bool block_on_flash,
                                      bool verify) {
  // uint32_t t = millis();

  // wait for NVM controller to be ready
  if (!updi_wait_flash_ready()) {
    DEBUG_VERBOSE("in updi_write_nvm() error: cant wait flash ready\n");
    return false;
  }
  // Serial.printf("wait flash 1: %d ms\n", millis()-t); t = millis();

  // Clear page buffer
  if (!updi_execute_nvm_command(UPDI_NVMCTRL_CTRLA_PAGE_BUFFER_CLR)) {
    DEBUG_VERBOSE("in updi_write_nvm() error: execute nvm command\n");
    return false;
  }

  // Serial.printf("clear page: %d ms\n", millis()-t); t = millis();

  // wait for NVM controller to be ready
  if (!updi_wait_flash_ready()) {
    DEBUG_VERBOSE("in updi_write_nvm() error: cant wait flash ready after page "
                  "buffer clear\n");
    return false;
  }

  // Serial.printf("wait flash 2: %d ms\n", millis()-t); t = millis();

  // Load the page buffer by writing directly to location
  // FYI: word access is significantly faster for write but no faster for read
  if (use_word_acess) {
    if (!updi_write_data_words(address, data, len >> 1)) {
      DEBUG_VERBOSE(
          "in updi_write_nvm() error: _updi_write_data_words() error\n");
      return false;
    }
  } else {
    if (!updi_write_data(address, data, len)) {
      DEBUG_VERBOSE("in updi_write_nvm() error: _updi_write_data() error\n");
      return false;
    }
  }

  // Serial.printf("write page: %d ms\n", millis()-t); t = millis();

  // Write the page to NVM, maybe erase first
  if (!updi_execute_nvm_command(command)) {
    DEBUG_VERBOSE("in updi_write_nvm() error: execute_nvm_command() error "
                  "committing page\n");
    return false;
  }

  // Serial.printf("nvm write: %d ms\n", millis()-t); t = millis();

  if (block_on_flash) {
    // wait for NVM controller to be ready
    if (!updi_wait_flash_ready()) {
      DEBUG_VERBOSE("in updi_write_nvm() error: cant wait flash ready after "
                    "commit page\n");
      return false;
    }

    // Serial.printf("wait flash 3: %d ms\n", millis()-t); t = millis();
  }
  if (verify) {
    uint8_t readbuf[len];
    if (!updi_read_data(address, readbuf, len)) {
      return false;
    }
    // Serial.printf("read buf: %d ms\n", millis()-t); t = millis();
    for (uint16_t b = 0; b < len; b++) {
      if (readbuf[b] != data[b]) {
        Serial.println("Verification error!");
        return false;
      }
    }
  }

  return true;
}
#endif
