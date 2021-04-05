#include "Adafruit_AVRProg.h"

const image_t PROGMEM image_calibration = {
    // Sketch name, only used for serial printing
    {"rc_calib_tiny85.hex"},
    // Chip name, only used for serial printing
    {"attiny85"},
    // Signature bytes for 328P
    0x930B,
    // Programming fuses, written before writing to flash. Fuses set to
    // zero are untouched.
    {0x03, 0xE2, 0xDD, 0xFF}, // {lock, low, high, extended}
    // Normal fuses, written after writing to flash (but before
    // verifying). Fuses set to zero are untouched.
    {0x0, 0x0, 0xD5, 0x0}, // {lock, low, high, extended}
    // Fuse verify mask. Any bits set to zero in these values are
    // ignored while verifying the fuses after writing them. All (and
    // only) bits that are unused for this atmega chip should be zero
    // here.
    {0x03, 0xFF, 0xFF, 0xFF}, // {lock, low, high, extended}
    // size of chip flash in bytes
    8192,
    // size in bytes of flash page
    64,
    // The actual image to flash. This can be copy-pasted as-is from a
    // .hex file. If you do, replace all lines below starting with a
    // colon, but make sure to keep the start and end markers {R"( and
    // )"} in place.
    {R"(
:1000000009C0FFFFFFFFFFFFFFFFFFFFFFFFFFFF35
:10001000FFFFFFFF002401E0102E0FE50DBF02E0FF
:100020000EBFC09AC19AB99A1CD000E00FBB00E085
:100030000EBB01B70DBB04E00CBB06E00CBB00001F
:10004000E199FECF18E002E026B320FFFDCF38B3E0
:10005000302726B320FDFDCF38BB1A95A9F7000045
:10006000FFCF662781E0000050E4650F61BF00000C
:10007000000031D0403069F15695552331F0052FFD
:10008000413009F40195600FF1CF413011F051E09A
:1000900001C05FEF650F61BF000000001CD0403061
:1000A000C1F0650F61BF0000000015D0403089F03D
:1000B0005195550F550F650F61BF000000000BD023
:1000C000403039F0882319F08A9560E8CDCFB99A8D
:1000D000C7CF462F0895332402E0402E06E202BE29
:1000E00011E0B09BFECFB099FECF13BF28B721FF20
:1000F00002C0310C48BEB09BFECF28B721FF02C022
:10010000310C48BEB099FECF28B721FF02C0310C98
:1001100048BEB09BFECF28B721FF02C0310C48BEBD
:10012000B099FECF0A9581F728B721FF02C0310CA4
:1001300048BEB09BFECF28B721FF02C0310C48BE9D
:10014000B099FECF03BE28B721FF04C00894301C2D
:10015000227028BF22B61FE726E2211632063CF4A1
:1001600011EC25E21215230524F440E0089541E046
:0601700008954FEF089511
:00000001FF
    )"}};

const image_t PROGMEM image_blinkLED = {
    // Sketch name, only used for serial printing
    {"blinkLED.hex"},
    // Chip name, only used for serial printing
    {"attiny85"},
    // Signature bytes for 328P
    0x930B,
    // Programming fuses, written before writing to flash. Fuses set to
    // zero are untouched.
    {0x03, 0xE2, 0xDD, 0xFF}, // {lock, low, high, extended}
    // Normal fuses, written after writing to flash (but before
    // verifying). Fuses set to zero are untouched.
    {0x0, 0x0, 0xD5, 0x0}, // {lock, low, high, extended}
    // Fuse verify mask. Any bits set to zero in these values are
    // ignored while verifying the fuses after writing them. All (and
    // only) bits that are unused for this atmega chip should be zero
    // here.
    {0x03, 0xFF, 0xFF, 0xFF}, // {lock, low, high, extended}
    // size of chip flash in bytes
    8192,
    // size in bytes of flash page
    64,
    // The actual image to flash. This can be copy-pasted as-is from a
    // .hex file. If you do, replace all lines below starting with a
    // colon, but make sure to keep the start and end markers {R"( and
    // )"} in place.
    {R"(
:100000000EC015C014C013C012C011C010C00FC064
:100010000EC00DC00CC00BC00AC009C008C011241E
:100020001FBECFE5D2E0DEBFCDBF02D01CC0E8CFFF
:10003000EFEFF3E0E491EF3F71F081B78E1720F41A
:1000400081B78F5F81BFF9CF81B7E81720F481B7FF
:10005000815081BFF9CFB99A82E18ABD83E083BF25
:0A0060008CE789BDFFCFF894FFCFB5
:00000001FF
    )"}};

/*
   Table of defined images
*/
const image_t *images[] = {
    &image_calibration,
    &image_blinkLED,
};

uint8_t NUMIMAGES = sizeof(images) / sizeof(images[0]);
