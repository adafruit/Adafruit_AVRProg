// UPDI fancy programming example with neopixel and fuse setting/unlocking

#include "Adafruit_AVRProg.h"
#include <Adafruit_NeoPixel.h>


Adafruit_AVRProg avrprog = Adafruit_AVRProg();
#define AVR_POWER                A2

Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, 40, NEO_GRB + NEO_KHZ800);
uint32_t stamp;

extern const image_t *images[];

void power_off() {
  pinMode(AVR_POWER, OUTPUT);
  digitalWrite(AVR_POWER, HIGH);
}
void power_on() {
  pinMode(AVR_POWER, OUTPUT);
  digitalWrite(AVR_POWER, LOW);
}
void power_cycle() {
  power_off();
  delay(100);
  power_on();
}

void print_time_taken(void) {
  Serial.print("Took (ms): "); 
  Serial.println(millis()-stamp);
  stamp = millis();
}

uint8_t allpins[] = {0, 1, 2, 3, 5, 6, 7, 8, 9, 12, 13, 14, 18, 20}; 

void setup() {
  Serial.begin(115200); /* Initialize serial for status msgs */
  //while (!Serial)  delay(10);

  power_cycle();

  Serial.println("\nAVRprog UPDI Test");

  pixel.begin();
  pixel.show(); // Initialize all pixels to 'off'
  pixel.setBrightness(20);

  avrprog.setUPDI(&Serial1, 115200*4, AVR_POWER, true);
  
  stamp = millis();

  pixel.setPixelColor(0, 0xFFFF00); pixel.show();
  
  while (!avrprog.targetPower(true)) {
    pixel.setPixelColor(0, 0xFF0000); pixel.show();
    Serial.println("Failed to connect to target, power cycling");
    power_cycle();
    delay(500);
  }
  Serial.println("UPDI Powered & Responded!");


  uint16_t signature = 0;
  while (1) {
    Serial.print(F("\nReading signature: "));
    signature = avrprog.readSignature();
    Serial.println(signature, HEX);
    pixel.setPixelColor(0, 0xFF0000); pixel.show();

    if (avrprog.g_updi.initialized && !avrprog.g_updi.unlocked) {
      Serial.println("OK so the chip is locked!");
      avrprog.UPDIunlock();
    } else if ((signature != 0) && signature != 0xFFFF) {
      break;
    } else {
      Serial.println(F("No target attached?"));
    }
    power_cycle();
    delay(500);
  }

  const image_t *targetimage = images[0];
  if (targetimage->image_chipsig != signature) {
    pixel.setPixelColor(0, 0xFF0000); pixel.show();
    avrprog.error(F("Signature doesn't match image"));
  }
  Serial.println(F("Found matching chip/image"));
  pixel.setPixelColor(0, 0xFF00FF); pixel.show();

  print_time_taken();

  Serial.print(F("\nReading fuses: "));
  uint8_t fuses[10];

  if (! avrprog.readFuses(fuses, 10)) {
    pixel.setPixelColor(0, 0xFF0000); pixel.show();
    avrprog.error(F("Couldn't read fuses"));
  }

  bool wrotefuse = false;
  for (uint8_t f=0; f<10; f++) {
    Serial.printf("Fuse #%d = 0x%02X", f, fuses[f]);
    uint8_t newfuse = targetimage->image_normfuses[f];
    if (newfuse != fuses[f]) {
      Serial.printf(" -> 0x%02X\n", newfuse);
      wrotefuse = true;
      if (! avrprog.programFuse(newfuse, f)) {
        pixel.setPixelColor(0, 0xFF0000); pixel.show();
        avrprog.error(F("Couldn't write fuses"));
      }
      avrprog.UPDIunlock();
    } else {
      Serial.println();
    }
  }
  if (wrotefuse) {
    //avrprog.unlocky();
  } else {
    Serial.print(F(" - Erasing chip..."));
    if (! avrprog.eraseChip()) {
      pixel.setPixelColor(0, 0xFF0000); pixel.show();
      avrprog.error(F("Failed to erase flash"));
    }
    Serial.println(F("Done!"));
  
    print_time_taken();
  }
  
  print_time_taken();

  if (!avrprog.writeImage(targetimage->image_hexcode,
                          pgm_read_byte(&targetimage->image_pagesize)*64,  // we'll 'bufferwrite' n pages at a time
                          pgm_read_word(&targetimage->chipsize))) {
    avrprog.error(F("Failed to write flash"));
  }
  Serial.println(F("Write OK!"));

  pixel.setPixelColor(0, 0xFFFFFF); pixel.show();
  print_time_taken();
}

void loop() {
}
