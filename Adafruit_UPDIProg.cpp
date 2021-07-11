#include "Adafruit_UPDIProg.h"

#define AVRDEBUG(fmt, ...)	Serial.printf(fmt, ##__VA_ARGS__)
#define DEBUG(fmt, ...)		Serial.printf(fmt, ##__VA_ARGS__)
#define VERBOSE(fmt, ...)	Serial.printf(fmt, ##__VA_ARGS__)



void Adafruit_AVRProg::updi_serial_init(void) {
  _updi_serial_retry_counter = 0;
  _updi_serial_retry_count = 0;

  uart->begin(115200, SERIAL_8E2);
  uart->setTimeout(50);
  VERBOSE("updi serial init set\n");

  _updi_serial_inited = true;
}


int Adafruit_AVRProg::updi_serial_read_wait(void) {
  int b = -1;
  _updi_serial_retry_counter++;
  _updi_serial_retry_count++;
  
  // try to wait for data
  while (_updi_serial_retry_counter++) {
    if (uart->available()) {
      b = uart->read();
      break;
    }
    delay(1);
    _updi_serial_retry_count++;
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
    VERBOSE("UPDISERIAL send count error %d != %d\n", count, size);
    return false;
	}
  delay(2);
  count = 0;
  for (uint16_t i = 0; i < size; i++) {
    b = updi_serial_read_wait(); // wait for data
    if (b != data[i]) {
      good_echo = false;
      VERBOSE("\tsend[%d] %02x != %02x\n", i, data[i], b);
    } else {
      VERBOSE("\tsend[%d] %02x == %02x\n", i, data[i], b);
    }
    count++;
  }
  if (count != size) {
    VERBOSE("UPDISERIAL echo count error %d != %d\n", count, size);
    return false;
  }
  return good_echo; // was return true
}


bool Adafruit_AVRProg::updi_serial_send_receive(uint8_t *data, uint16_t size, uint8_t *buff, uint32_t len) {
	/*
		NOTE: since the TX and RX pins are tied together,
		everything we send gets echo'd and needs to be
		discarded QUICKLY.
	*/
	bool timeout = false;
	uint32_t count = 0;
	int b;

	if (updi_serial_send(data, size)) {
		for (uint32_t i = 0; i < len; i++) {
			b = updi_serial_read_wait(); // wait for data
			buff[count++] = b;
			if (b == -1)
				timeout = true;
			VERBOSE("receive %d of %d =  %02x\n", i + 1, len, b);
		}
		if (count != len) {
			VERBOSE("UPDISERIAL receive count error %d != %d\n", count, len);
			return false;
		}
		if (timeout) {
			VERBOSE("UPDISERIAL timeout while reading data\n");
			return false;
		}
		return true;
	}
	return false;
}



bool Adafruit_AVRProg::udpi_stcs(uint8_t address, uint8_t value) {
  uint8_t buf[3] = {UPDI_PHY_SYNC, (uint8_t)(UPDI_STCS | (address & 0x0F)), value};
  VERBOSE("STCS(%02X) %02X %02X %02X\n", value, buf[0], buf[1], buf[2]);
  
  return updi_serial_send(buf, 3);
}

//Load data from Control/Status space
uint8_t Adafruit_AVRProg::updi_ldcs(uint8_t address) {

	uint8_t buf[2] = {UPDI_PHY_SYNC, (uint8_t)(UPDI_LDCS | (address & 0x0F))};
	uint8_t recv = 0;
	AVRDEBUG("LDCS %02X %02X  ", buf[0], buf[1]);

	if (!updi_serial_send_receive(buf, 2, &recv, 1)) {
		VERBOSE("updi_ldcs error\n");
		return 0;
	} else {
		AVRDEBUG("%02X\n", recv);
		return recv;
	}
}

bool Adafruit_AVRProg::updi_check(void) {
	VERBOSE("updi_check()\n");
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

	VERBOSE("Sending BREAK BREAK\n");
	/*
		The BREAK character is used to reset the internal state of the UPDI to the default setting.
		This is useful if the UPDI enters an Error state due to a communication error or when the
		synchronization between the debugger and the UPDI is lost.

		To ensure that a BREAK is successfully received by the UPDI in all cases, the debugger
		must send two consecutive BREAK characters. The first BREAK will be detected if the UPDI
		is in Idle state and will not be detected if it is sent while the UPDI is receiving or
		transmitting (at a very low baud rate). However, this will cause a frame error for the
		reception (RX) or a contention error for the transmission (TX), and abort the ongoing
		operation. The UPDI will then detect the next BREAK successfully.

		The minimum BREAK is 6ms (a 20Mhz oscillator) and the worst-case is 25ms.

		We could calculate the optimal BREAK using details about the chip ... or not
	*/

    updi_serial_force_break();
	return true;
}

void Adafruit_AVRProg::updi_serial_force_break(void) {
	VERBOSE("updi_serial_force_break()\n");

	// flush anything
	while (uart->available())
		uart->read();

	VERBOSE("updi_serial baud 110\n");
	uart->begin(110);
	delay(50);

	VERBOSE("updi_serial BREAK 1\n");
	uart->write((byte)0);
	// flush anything
	while (uart->available())
		uart->read();

	delay(12);

	VERBOSE("updi_serial BREAK 2\n");
	uart->write((byte)0);
	while (uart->available())
		uart->read();

	VERBOSE("updi_serial baud 115200\n");
	uart->begin(115200);
}
