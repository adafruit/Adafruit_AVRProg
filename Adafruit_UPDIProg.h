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

#ifndef _ADAFRUIT_UPDIPROG_H
#define _ADAFRUIT_UPDIPROG_H

#include <Arduino.h>

#if defined(__AVR__) || defined(ESP8266) ||                                    \
    (defined(ARDUINO_ARCH_SAMD) && !defined(ARDUINO_SAMD_ADAFRUIT))
//#pragma message("UPDI Support NOT included")
#else
//#pragma message("UPDI Support included")
#define SUPPORT_UPDI
#endif

// clang-format off

//#define AVRDEBUG(fmt, ...)	        {Serial.print("\t[AVR]\t"); Serial.printf(fmt, ##__VA_ARGS__);}
#define AVRDEBUG(fmt, ...) {}

//#define DEBUG(fmt, ...)		Serial.printf(fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...) {}

//#define DEBUG_TASK(fmt, ...)		Serial.printf(fmt, ##__VA_ARGS__)
#define DEBUG_TASK(fmt, ...) {}

//#define DEBUG_VERBOSE(fmt, ...)	        Serial.printf(fmt, ##__VA_ARGS__)
#define DEBUG_VERBOSE(fmt, ...) {}

//#define DEBUG_PHYSICAL(fmt, ...)	Serial.printf(fmt, ##__VA_ARGS__)
#define DEBUG_PHYSICAL(fmt, ...) {}

#define DEBUG_FUSES(fmt, ...)	Serial.printf(fmt, ##__VA_ARGS__)
//#define DEBUG_FUSES(fmt, ...) {}

// clang-format on

#define UPDI_BREAK 0x00

#define UPDI_LDS 0x00
#define UPDI_STS 0x40
#define UPDI_LD 0x20
#define UPDI_ST 0x60
#define UPDI_LDCS 0x80
#define UPDI_STCS 0xC0
#define UPDI_REPEAT 0xA0
#define UPDI_KEY 0xE0

#define UPDI_PTR 0x00
#define UPDI_PTR_INC 0x04
#define UPDI_PTR_ADDRESS 0x08

#define UPDI_ADDRESS_8 0x00
#define UPDI_ADDRESS_16 0x04

#define UPDI_DATA_8 0x00
#define UPDI_DATA_16 0x01

#define UPDI_KEY_SIB 0x04
#define UPDI_KEY_KEY 0x00

#define UPDI_KEY_64 0x00
#define UPDI_KEY_128 0x01

#define UPDI_SIB_8BYTES UPDI_KEY_64
#define UPDI_SIB_16BYTES UPDI_KEY_128

#define UPDI_REPEAT_BYTE 0x00
#define UPDI_REPEAT_WORD 0x01

#define UPDI_PHY_SYNC 0x55
#define UPDI_PHY_ACK 0x40

#define UPDI_MAX_REPEAT_SIZE 0xFF

// TASKS
#define UPDI_TASK_ERASE 1
#define UPDI_TASK_READ_FUSES 2
#define UPDI_TASK_WRITE_FUSES 4
#define UPDI_TASK_READ_FLASH 8
#define UPDI_TASK_WRITE_FLASH 16
#define UPDI_TASK_VERIFY_FLASH 32
#define UPDI_TASK_READ_EEPROM 64
#define UPDI_TASK_WRITE_EEPROM 128
#define UPDI_TASK_GET_INFO 256
#define UPDI_TASK_WRITE_USERROW 512
#define UPDI_TASKS                                                             \
  (UPDI_TASK_GET_INFO | UPDI_TASK_READ_FUSES | UPDI_TASK_WRITE_FUSES |         \
   UPDI_TASK_READ_FLASH | UPDI_TASK_ERASE | UPDI_TASK_WRITE_FLASH |            \
   UPDI_TASK_READ_EEPROM | UPDI_TASK_WRITE_EEPROM | UPDI_TASK_WRITE_USERROW)

// CS and ASI Register Address map
#define UPDI_CS_STATUSA 0x00
#define UPDI_CS_STATUSB 0x01
#define UPDI_CS_CTRLA 0x02
#define UPDI_CS_CTRLB 0x03
#define UPDI_ASI_KEY_STATUS 0x07
#define UPDI_ASI_RESET_REQ 0x08
#define UPDI_ASI_CTRLA 0x09
#define UPDI_ASI_SYS_CTRLA 0x0A
#define UPDI_ASI_SYS_STATUS 0x0B
#define UPDI_ASI_CRC_STATUS 0x0C

#define UPDI_CTRLA_IBDLY_BIT 7
#define UPDI_CTRLA_RSD_BIT 3
#define UPDI_CTRLB_CCDETDIS_BIT 3
#define UPDI_CTRLB_UPDIDIS_BIT 2

#define UPDI_KEY_NVM "NVMProg "
#define UPDI_KEY_CHIPERASE "NVMErase"
#define UPDI_KEY_USERROW_WRITE "NVMUs&te"

#define UPDI_ASI_STATUSA_REVID 4
#define UPDI_ASI_STATUSB_PESIG 0

#define UPDI_ASI_KEY_STATUS_CHIPERASE 3
#define UPDI_ASI_KEY_STATUS_NVMPROG 4
#define UPDI_ASI_KEY_STATUS_UROWWRITE 5

#define UPDI_ASI_SYS_STATUS_RSTSYS 5
#define UPDI_ASI_SYS_STATUS_INSLEEP 4
#define UPDI_ASI_SYS_STATUS_NVMPROG 3
#define UPDI_ASI_SYS_STATUS_UROWPROG 2
#define UPDI_ASI_SYS_STATUS_LOCKSTATUS 0

#define UPDI_ASI_SYS_CTRLA_UROWupdi_write_FINAL 1
#define UPDI_ASI_SYS_CTRLA_CLKREQ 0

#define UPDI_RESET_REQ_VALUE 0x59

// FLASH CONTROLLER
#define UPDI_NVMCTRL_CTRLA 0x00
#define UPDI_NVMCTRL_CTRLB 0x01
#define UPDI_NVMCTRL_STATUS 0x02
#define UPDI_NVMCTRL_INTCTRL 0x03
#define UPDI_NVMCTRL_INTFLAGS 0x04
#define UPDI_NVMCTRL_DATAL 0x06
#define UPDI_NVMCTRL_DATAH 0x07
#define UPDI_NVMCTRL_ADDRL 0x08
#define UPDI_NVMCTRL_ADDRH 0x09

// CTRLA
#define UPDI_NVMCTRL_CTRLA_NOP 0x00
#define UPDI_NVMCTRL_CTRLA_updi_write_PAGE 0x01
#define UPDI_NVMCTRL_CTRLA_ERASE_PAGE 0x02
#define UPDI_NVMCTRL_CTRLA_ERASE_updi_write_PAGE 0x03
#define UPDI_NVMCTRL_CTRLA_PAGE_BUFFER_CLR 0x04
#define UPDI_NVMCTRL_CTRLA_updi_erase_chip 0x05
#define UPDI_NVMCTRL_CTRLA_ERASE_EEPROM 0x06
#define UPDI_NVMCTRL_CTRLA_UPDI_WRITE_FUSE 0x07

#define UPDI_NVM_STATUS_UPDI_WRITE_ERROR 2
#define UPDI_NVM_STATUS_EEPROM_BUSY 1
#define UPDI_NVM_STATUS_FLASH_BUSY 0

#define UPDI_MAX_FILENAME_LEN 256
#define AVR_SYSCFG_ADDRESS 0x0F00
#define AVR_OCD_ADDRESS 0x0F80
#define AVR_NVM_ADDRESS 0x1000
#define AVR_SIG_ADDRESS 0x1100
#define AVR_FUSES_ADDRESS 0x1280
#define AVR_USERSIG_ADDRESS 0x1300
#define AVR_EEPROM_ADDR 0x1400
#define AVR_FLASH_ADDR // varies with chip size
#define AVR_UID_LENGTH 10

// FUSE definitions
#define AVR_FUSE_UNINITIALIZED                                                 \
  0xFF // TODO need to find a value that is guaranteed to never be used
#define AVR_NUM_FUSES                                                          \
  11 // 3 fuses are marked 'reserved': the 4th, 5th, and 10th (0-based #3, #4,
     // and #9)
#define AVR_FUSE_BASE 0x1280
#define AVR_FUSE_WDTCFG 0x1280 // watch dog timer
#define AVR_FUSE_BODCFG 0x1281 // brown out detection
#define AVR_FUSE_OSCCFG 0x1282 // oscillator frequency
#define AVR_FUSE_RESV3 0x1283  // must be 0xFF
#define AVR_FUSE_RESV4 0x1284
#define AVR_FUSE_SYSCFG0 0x1285
#define AVR_FUSE_SYSCFG1 0x1286
#define AVR_FUSE_APPEND 0x1287
#define AVR_FUSE_BOOTEND 0x1288
#define AVR_FUSE_RESV9 0x1289
#define AVR_FUSE_LOCK 0x128A

#define AVR_PAGESIZE_MAX                                                       \
  256 // it is probably 128 but I have not checked all possible chips

/*! Detailed info about chip */
typedef struct {
  uint16_t signature; ///< all signatures begin with 0x1E and then 2 bytes
  char shortname[8];  ///< smol name
  char longname[16];  ///< long name
  uint8_t config;     ///< config byte
} DeviceIdentification;

// AVR configurations
#define AVR8X_TINY_2X 0
#define AVR8X_TINY_4X 1
#define AVR8X_TINY_8X 2
#define AVR8X_TINY_16X 3
#define AVR8X_MEGA_320 4
#define AVR8X_MEGA_321 5
#define AVR8X_MEGA_480 6

/*! Device configuration struct */
typedef struct {
  uint32_t flash_start;     ///< Flash mem offset start (e.g. 0x8000)
  uint32_t flash_size;      ///< Flash size in bytes
  uint8_t flash_pagesize;   ///< Page size in bytes
  uint16_t eeprom_size;     ///< EEPROM size in bytes
  uint16_t eeprom_pagesize; ///< EEPROM page in bytes
} DeviceConfiguration;

/*! data read from chip */
typedef struct {
  char family[8];             ///< Family name
  char nvm_version[4];        ///< NVM version
  char ocd_version[4];        ///< OCD version
  uint8_t signature_bytes[3]; ///< Chip signature bytes
  char dbg_osc_freq;          ///< Debug freq
  uint8_t pdi_rev;            ///< PDI rev
  uint8_t dev_rev;            ///< Devices rev
  int8_t error_16v3;          ///< Errors
  int8_t error_16v5;          ///< Errors
  int8_t error_20v3;          ///< Errors
  int8_t error_20v5;          ///< Errors
  uint8_t uid[10];            ///< UID
} DeviceDetails;

/*! UPDI status struct */
typedef struct {
  bool initialized;             ///< Chip inited?
  bool unlocked;                ///< Chip unlocked?
  bool verified;                ///< Chip verified?
  DeviceIdentification *device; ///< Device info
  DeviceConfiguration *config;  ///< devince config
  DeviceDetails details;        ///< this is data read from device
  uint8_t fuses[AVR_NUM_FUSES]; ///< Fuse memory
} UPDI;

#endif
