/***************************************************************************
  This is a library for the BMP280 pressure sensor

  Designed specifically to work with the Adafruit BMP280 Breakout
  ----> http://www.adafruit.com/products/2651

  These sensors use I2C to communicate, 2 pins are required to interface.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ***************************************************************************/

#include "Adafruit_BMP280.h"

/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

TwoWire i2c(I2C_DEV, I2C_FAST_MODE); 

Adafruit_BMP280::Adafruit_BMP280()
{ }


bool Adafruit_BMP280::begin() {

  i2c.begin();

  if (read(BMP280_REGISTER_CHIPID, 1) != 0x58)
    return false;

  readCoefficients();

  i2c.beginTransmission(BMP280_ADDRESS);
  i2c.write(BMP280_REGISTER_CONTROL);
  i2c.write(0x3F);
  i2c.endTransmission();

  return true;
}


uint16_t Adafruit_BMP280::read16_LE(byte reg) {

  uint16_t temp = read(reg, 2);
  return (temp >> 8) | (temp << 8);

}


/**************************************************************************/
/*!
    @brief  Reads a signed 16 bit value over I2C
*/
/**************************************************************************/

uint32_t Adafruit_BMP280::read(byte reg, byte nBytes)
{
  uint32_t value;

  i2c.beginTransmission(BMP280_ADDRESS);
  i2c.write((uint8_t)reg);
  i2c.endTransmission();
  i2c.requestFrom(BMP280_ADDRESS, nBytes);
    
  value = i2c.read();

  for (int i = 1; i < nBytes; ++i) {
	  value <<= 8;
	  value |= i2c.read();
  }

  return value;
}

/**************************************************************************/
/*!
    @brief  Reads the factory-set coefficients
*/
/**************************************************************************/
void Adafruit_BMP280::readCoefficients(void)
{
    _bmp280_calib.dig_T1 = read16_LE(BMP280_REGISTER_DIG_T1);
    _bmp280_calib.dig_T2 = (int16_t) read16_LE(BMP280_REGISTER_DIG_T2);
    _bmp280_calib.dig_T3 = (int16_t) read16_LE(BMP280_REGISTER_DIG_T3);

    _bmp280_calib.dig_P1 = read16_LE(BMP280_REGISTER_DIG_P1);
    _bmp280_calib.dig_P2 = (int16_t) read16_LE(BMP280_REGISTER_DIG_P2);
    _bmp280_calib.dig_P3 = (int16_t) read16_LE(BMP280_REGISTER_DIG_P3);
    _bmp280_calib.dig_P4 = (int16_t) read16_LE(BMP280_REGISTER_DIG_P4);
    _bmp280_calib.dig_P5 = (int16_t) read16_LE(BMP280_REGISTER_DIG_P5);
    _bmp280_calib.dig_P6 = (int16_t) read16_LE(BMP280_REGISTER_DIG_P6);
    _bmp280_calib.dig_P7 = (int16_t) read16_LE(BMP280_REGISTER_DIG_P7);
    _bmp280_calib.dig_P8 = (int16_t) read16_LE(BMP280_REGISTER_DIG_P8);
    _bmp280_calib.dig_P9 = (int16_t) read16_LE(BMP280_REGISTER_DIG_P9);
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
float Adafruit_BMP280::readTemperature(void)
{
  int32_t var1, var2;

  int32_t adc_T = read(BMP280_REGISTER_TEMPDATA, 3);
  adc_T >>= 4;

  var1  = ((((adc_T>>3) - ((int32_t)_bmp280_calib.dig_T1 <<1))) *
	   ((int32_t)_bmp280_calib.dig_T2)) >> 11;

  var2  = (((((adc_T>>4) - ((int32_t)_bmp280_calib.dig_T1)) *
	     ((adc_T>>4) - ((int32_t)_bmp280_calib.dig_T1))) >> 12) *
	   ((int32_t)_bmp280_calib.dig_T3)) >> 14;

  t_fine = var1 + var2;

  int T  = (t_fine * 5 + 128) >> 8;
  return T / 100;
}

/**************************************************************************/
/*!

*/
/**************************************************************************/
float Adafruit_BMP280::readPressure(void) {
  int64_t var1, var2, p;

  // Must be done first to get the t_fine variable set up
  //readTemperature();

  int32_t adc_P = read(BMP280_REGISTER_PRESSUREDATA, 3);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)_bmp280_calib.dig_P6;
  var2 = var2 + ((var1*(int64_t)_bmp280_calib.dig_P5)<<17);
  var2 = var2 + (((int64_t)_bmp280_calib.dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)_bmp280_calib.dig_P3)>>8) +
    ((var1 * (int64_t)_bmp280_calib.dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)_bmp280_calib.dig_P1)>>33;

  if (var1 == 0) {
    return 0;  // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p<<31) - var2)*3125) / var1;
  var1 = (((int64_t)_bmp280_calib.dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)_bmp280_calib.dig_P8) * p) >> 19;

  p = ((p + var1 + var2) >> 8) + (((int64_t)_bmp280_calib.dig_P7)<<4);
  return p/256;
}
