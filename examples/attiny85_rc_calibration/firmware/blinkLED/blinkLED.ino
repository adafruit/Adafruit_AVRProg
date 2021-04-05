#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/eeprom.h>

int main() {
  unsigned char osccal_calibrated = pgm_read_byte(0x3FF);
  if (osccal_calibrated != 0xFF) {
    while (osccal_calibrated > OSCCAL) OSCCAL++;
    while (osccal_calibrated < OSCCAL) OSCCAL--;
  }
  DDRB |= (1 << PB1);
  TCCR0A = (0b01 << COM0B0) | (0b10 << WGM00); //CTC mode
  TCCR0B = (0b011 << CS00); //8M/64/125=1KHz
  OCR0A = 124;

  while (1) {

  }
  return 1;
}
