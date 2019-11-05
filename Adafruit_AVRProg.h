#ifndef _ADAFRUIT_AVRPROG_H
#define _ADAFRUIT_AVRPROG_H

#include <Arduino.h>
#include <SPI.h>
#ifdef __AVR__
#include <avr/pgmspace.h>
#endif

#define FUSE_PROT 0 ///< memory protection
#define FUSE_LOW 1  ///< Low fuse
#define FUSE_HIGH 2 ///< High fuse
#define FUSE_EXT 3  ///< Extended fuse

//#define VERBOSE 2

#define FUSE_CLOCKSPEED 10000    ///< Fuses need to be programmed slowly
#define FLASH_CLOCKSPEED 1000000 ///< Once fuse'd you can flash fast!

/**! Struct for holding one 'program' fuses & code */
typedef struct image {
  char image_name[30];     ///< i.e. "optiboot_diecimila.hex"
  char image_chipname[12]; ///< i.e. "atmega168"
  uint16_t image_chipsig;  ///< Low two bytes of signature, check datasheet!
  byte image_progfuses[5]; ///< fuses to set during programming (e.g unlock)
  byte image_normfuses[5]; ///< fuses to set after programming (e.g lock)
  byte fusemask[4]; ///< Not all bits are used in the fuses, mask the ones we do
                    ///< use
  uint16_t chipsize;   ///< Total size for flash programming, in bytes. check
                       ///< datasheet!
  byte image_pagesize; ///< Page size for flash programming, in bytes. check
                       ///< datasheet!
#ifdef __SAMD21G18A__
  byte image_hexcode[50000]; ///< Max buffer for intel hex format image (text)
#else
  byte image_hexcode[10000]; ///< Max buffer for intel hex format image (text)
#endif
} image_t;

// Useful message printing definitions

#define debug(string) // Serial.println(string);

/*!
  @brief    Class for talking to an AVR chip with SPI to program flash and fuses
*/
class Adafruit_AVRProg {

public:
  Adafruit_AVRProg();

  void pulseLED(int pin, int times);
  void eraseChip(void);
  bool programFuses(const byte *fuses);
  bool verifyFuses(const byte *fuses, const byte *fusemask);
  bool writeImage(const byte *hextext, uint8_t pagesize, uint32_t chipsize);
  bool verifyImage(const byte *hextext);

  void setSPI(int8_t reset_pin, SPIClass *theSPI = &SPI);
  void setSPI(int8_t reset_pin, int8_t sck_pin, int8_t mosi_pin,
              int8_t miso_pin);

  /*!
    @brief  Set up a GPIO as a programming-indicator LED
    @param led The pin to use for the LED */
  void setProgramLED(uint8_t led) { progLED = led; }
  /*!
    @brief  Set up a GPIO as an error-indicator LED
    @param led The pin to use for the LED */
  void setErrorLED(uint8_t led) { errLED = led; }
  void generateClock(void);

  bool targetPower(bool poweron);
  uint16_t readSignature(void);

  void error(const char *string);
  void error(const __FlashStringHelper *string);

private:
  bool startProgramMode(uint32_t clockrate = 100000);
  void endProgramMode(void);

  uint32_t isp_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
  uint8_t transfer(uint8_t out);
  void busyWait(void);

  bool flashPage(byte *pagebuff, uint16_t pageaddr, uint8_t pagesize);
  bool flashWord(uint8_t hilo, uint16_t addr, uint8_t data);
  const byte *readImagePage(const byte *hextext, uint16_t pageaddr,
                            uint8_t pagesize, byte *page);
  byte hexToByte(byte h);

  int8_t _reset, _mosi, _miso, _sck;
  uint16_t spiBitDelay;
  int8_t progLED, errLED;
  SPIClass *spi;
  bool programmode;
};

#endif
