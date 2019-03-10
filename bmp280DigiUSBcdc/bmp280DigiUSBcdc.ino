/*
 Name:		bmp280DigiUSBcdc.ino
 Created:	09/03/2019 11:17:05
 Author:	jjbubka
*/

#include <DigiCDC.h>

#include "Adafruit_BMP280.h"

Adafruit_BMP280 bme; // I2C

uint8_t received, command;
int32_t cTemp, cPress;
unsigned long lastUpdate;

void setup() {
	// initialize the digital pin as an output.
	SerialUSB.begin();
	bme.begin();
	cTemp = bme.readTemperature();
	cPress = bme.readPressure();
	lastUpdate = millis();
}

// the loop routine runs over and over again forever:
void loop() {

	
	if (millis() - lastUpdate >= 500) {
		lastUpdate = millis();
		cTemp = (cTemp * 90 + bme.readTemperature() * 10) / 100;
		cPress = (cPress *  90 + bme.readPressure() * 10) / 100;
	}

	
	while (SerialUSB.available()) {
		received = SerialUSB.read();
		if (command == 'p') {
			if (received == 10) {
				command = 0;
				SerialUSB.println(cPress);
			}
			else if (received != 13) {
				command = 0;
			}

		}
		else if (command == 't') {
			if (received == 10) {
				command = 0;
				SerialUSB.println(cTemp);
			}
			else if (received != 13) {
				command = 0;
			}
		}


	}
	
	
	//SerialUSB.delay(10);
	/*
	if you don't call a SerialUSB function (write, print, read, available, etc)
	every 10ms or less then you must throw in some SerialUSB.refresh();
	for the USB to keep alive - also replace your delays - ie. delay(100);
	with SerialUSB.delays ie. SerialUSB.delay(100);
	*/
}
