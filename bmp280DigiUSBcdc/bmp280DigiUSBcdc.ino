/*
 Name:		bmp280DigiUSBcdc.ino
 Created:	09/03/2019 11:17:05
 Author:	jjbubka
 Platform:  STM32F103
*/

#include <Wire.h>

#include "Adafruit_BMP280.h"

Adafruit_BMP280 bme; // I2C

uint8_t received, command;
float cTemp, cPress;
unsigned long lastUpdate;


void setup() {
	// initialize the digital pin as an output.
	Serial.begin();
	pinMode(PC13, OUTPUT);
	bme.begin();
	cTemp = bme.readTemperature();
	cPress = bme.readPressure();

	lastUpdate = millis();
}

// the loop routine runs over and over again forever:
void loop() {

	
	if (millis() - lastUpdate >= 500) {
		lastUpdate = millis();
		cTemp = cTemp * 0.95 + bme.readTemperature() * 0.05;
		if (cPress < 100000)
			cPress = bme.readPressure();
		else {
			cPress = cPress * 0.95 + bme.readPressure() * 0.05;
		}
		digitalWrite(PC13, !digitalRead(PC13));
	}

	
	while (Serial.available()) {
		received = Serial.read();
		if (command == 'p') {
			if (received == 10) {
				command = 0;
				Serial.println(cPress);
			}
			else if (received != 13) {
				command = 0;
			}

		}
		else if (command == 't') {
			if (received == 10) {
				command = 0;
				Serial.println(cTemp);
			}
			else if (received != 13) {
				command = 0;
			}
		}
		else
			command = received;


	}
	
}
