#include "Adafruit_AVRProg.h"

Adafruit_AVRProg avrprog = Adafruit_AVRProg();
#define AVR_POWER                A2

uint32_t stamp;

extern const image_t *images[];

void print_time_taken(void) {
  Serial.print("Took (ms): "); 
  Serial.println(millis()-stamp);
  stamp = millis();
}

void setup() {
  Serial.begin(115200); /* Initialize serial for status msgs */
  while (!Serial)
    delay(10);
  delay(100);
  Serial.println("\nAVRprog UPDI Test");

  avrprog.setUPDI(&Serial1, 115200, AVR_POWER); // can also try 230400
  
  stamp = millis();

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

  print_time_taken();

  Serial.print(F("\nReading fuses: "));
  uint8_t fuses[10];
  if (! avrprog.readFuses(fuses, 10)) {
    avrprog.error(F("Couldn't read fuses"));
  }
  for (uint8_t i=0; i<10; i++) {
    Serial.print("0x"); Serial.print(fuses[i], HEX); Serial.print(", ");
  }
  Serial.println();

  print_time_taken();

  Serial.print(F(" - Erasing chip..."));
  if (! avrprog.eraseChip()) {
    avrprog.error(F("Failed to erase flash"));
  }
  Serial.println(F("Done!"));

  print_time_taken();

  if (!avrprog.writeImage(targetimage->image_hexcode,
                          pgm_read_byte(&targetimage->image_pagesize)*64,  // we'll 'bufferwrite' n pages at a time
                          pgm_read_word(&targetimage->chipsize))) {
    avrprog.error(F("Failed to write flash"));
  }
  Serial.println(F("Write OK!"));

  print_time_taken();
}



void loop() {
 
}
