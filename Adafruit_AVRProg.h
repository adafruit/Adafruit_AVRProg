#ifndef _ADAFRUIT_AVRPROG_H
#define _ADAFRUIT_AVRPROG_H

#include <Arduino.h>
#include <SPI.h>
#ifdef __AVR__
  #include <avr/pgmspace.h>
#endif

#define FUSE_PROT 0			/* memory protection */
#define FUSE_LOW 1			/* Low fuse */
#define FUSE_HIGH 2			/* High fuse */
#define FUSE_EXT 3			/* Extended fuse */

//#define VERBOSE 1

#define FUSE_CLOCKSPEED         100000
#define FLASH_CLOCKSPEED        1000000

typedef struct image {
    char image_name[30];	       /* Ie "optiboot_diecimila.hex" */
    char image_chipname[12];	       /* ie "atmega168" */
    uint16_t image_chipsig;	       /* Low two bytes of signature */
    byte image_progfuses[5];	       /* fuses to set during programming */
    byte image_normfuses[5];	       /* fuses to set after programming */
    byte fusemask[4];
    uint16_t chipsize;
    byte image_pagesize;	       /* page size for flash programming */
#ifdef __SAMD21G18A__
    byte image_hexcode[50000];	       /* intel hex format image (text) */
#else
    byte image_hexcode[19000];	       /* intel hex format image (text) */
#endif
} image_t;

typedef struct alias {
  char image_chipname[12];
  uint16_t image_chipsig;
  image_t * alias_image;
} alias_t;

// Useful message printing definitions

#define debug(string) //Serial.println(string);

class Adafruit_AVRProg {

 public:
  Adafruit_AVRProg();
  void begin();

  void pulseLED(int pin, int times);
  void eraseChip(void);
  bool programFuses (const byte *fuses);
  bool verifyFuses (const byte *fuses, const byte *fusemask);
  bool writeImage (const byte *hextext, uint8_t pagesize, uint32_t chipsize);
  bool verifyImage (const byte *hextext);

  void setSPI(int8_t reset_pin, SPIClass *theSPI=&SPI);
  void setSPI(int8_t reset_pin, int8_t sck_pin, int8_t mosi_pin, int8_t miso_pin);
  void setProgramLED(uint8_t led) { progLED = led; }
  void setErrorLED(uint8_t led) { errLED = led; }
  void generateClock(void);

  bool targetPower(bool poweron);
  uint16_t readSignature(void);


  void error(const char *string);
  void error(const __FlashStringHelper *string);

 private:
  bool startProgramMode(uint32_t clockrate=100000);
  void endProgramMode(void);

  uint16_t isp_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
  uint8_t transfer(uint8_t out);
  void busyWait(void);

  bool flashPage (byte *pagebuff, uint16_t pageaddr, uint8_t pagesize);
  bool flashWord (uint8_t hilo, uint16_t addr, uint8_t data);
  const byte *readImagePage (const byte *hextext, uint16_t pageaddr, uint8_t pagesize, byte *page);
  byte hexToByte(byte h);

  int8_t _reset, _mosi, _miso, _sck;
  uint16_t spiBitDelay;
  int8_t progLED, errLED;
  SPIClass *spi;
  bool programmode;
};

#endif
