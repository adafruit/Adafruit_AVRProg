#include "Adafruit_AVRProg.h"

Adafruit_AVRProg avrprog = Adafruit_AVRProg();

/*
 * Pins to target
 */
#define AVRPROG_SCK 13
#define AVRPROG_MISO 12
#define AVRPROG_MOSI 11
#define AVRPROG_RESET 10

#define LED_PROGMODE LED_BUILTIN
#define LED_ERR LED_BUILTIN

#define BUTTON AVRPROG_RESET // use the board's reset button!
#define PIEZOPIN 8

extern const image_t *images[];

void setup() {
  Serial.begin(115200); /* Initialize serial for status msgs */

  while (!Serial)
    ;
  delay(100);

  Serial.println("\nAdaBootLoader Bootstrap programmer (originally OptiLoader "
                 "Bill Westfield (WestfW))");

  pinMode(PIEZOPIN, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP); // button for next programming

  avrprog.setProgramLED(LED_PROGMODE);
  avrprog.setErrorLED(LED_ERR);
  // avrprog.generateClock();
  avrprog.setSPI(AVRPROG_RESET, AVRPROG_SCK, AVRPROG_MOSI, AVRPROG_MISO);
}

void loop(void) {
  Serial.println("\nType 'G' or hit BUTTON for next chip");
  while (1) {
    if (Serial.read() == 'G') {
      break;
    }
    if (!digitalRead(BUTTON)) {
      while (!digitalRead(BUTTON)) {
        delay(10);
      }          // wait for release
      delay(10); // debounce
      break;
    }
  }

  if (!avrprog.targetPower(true)) {
    avrprog.error("Failed to connect to target");
  }

  Serial.print(F("\nReading signature: "));
  uint16_t signature = avrprog.readSignature();
  Serial.println(signature, HEX);
  if (signature == 0 || signature == 0xFFFF) {
    avrprog.error(F("No target attached?"));
  }

  const image_t *targetimage = images[0];
  if (targetimage->image_chipsig != signature) {
    avrprog.error(F("Signature doesn't match image"));
  }
  Serial.println(F("Found matching chip/image"));

  Serial.print(F("Erasing chip..."));
  avrprog.eraseChip();
  Serial.println(F("Done!"));

  if (!avrprog.programFuses(
          targetimage->image_progfuses)) { // get fuses ready to program
    avrprog.error(F("Programming Fuses fail"));
  }
  
  delay(50);

  if (!avrprog.verifyFuses(targetimage->image_progfuses,
                           targetimage->fusemask)) {
    avrprog.error("Failed to verify fuses");
  } else {
    Serial.println("Fuses verified correctly!");
  }
  
  // We should disconnect/reconnect after fusing
  avrprog.targetPower(false);
  delay(100);
  if (!avrprog.targetPower(true)) {
    avrprog.error("Failed to connect to target");
  }

  if (!avrprog.writeImage(targetimage->image_hexcode,
                          pgm_read_byte(&targetimage->image_pagesize),
                          pgm_read_word(&targetimage->chipsize))) {
    avrprog.error(F("Failed to write flash"));
  }

  Serial.println(F("\nVerifing flash..."));
  if (!avrprog.verifyImage(targetimage->image_hexcode)) {
    avrprog.error(F("Failed to verify flash"));
  }
  Serial.println(F("\nFlash verified correctly!"));

  // Set fuses to 'final' state
  if (!avrprog.programFuses(targetimage->image_normfuses)) {
    avrprog.error("Programming fuses fail");
  }

  if (!avrprog.verifyFuses(targetimage->image_normfuses,
                           targetimage->fusemask)) {
    avrprog.error("Failed to verify fuses");
  } else {
    Serial.println("Fuses verified correctly!");
  }

#if !defined(ESP32)
  // no 'tone' on ESP32
  tone(PIEZOPIN, 4000, 200);
#endif

  while (1)
    ;
}