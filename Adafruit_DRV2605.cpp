/*************************************************** 
  This is a library for the Adafruit DRV2605L Haptic Driver

  ----> http://www.adafruit.com/products/2306
 
  Check out the links above for our tutorials and wiring diagrams
  This motor/haptic driver uses I2C to communicate

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

#include <Adafruit_DRV2605.h>

/**************************************************************************/
/*! 
    @brief  Instantiates a new DRV2605 class
*/
/**************************************************************************/
// I2C, no address adjustments or pins
Adafruit_DRV2605::Adafruit_DRV2605() {
}


/**************************************************************************/
/*! 
    @brief  Setups the HW
*/
/**************************************************************************/
boolean Adafruit_DRV2605::begin() {
  Wire.begin();
  // uint8_t id = readRegister8(DRV2605_REG_STATUS);
  //Serial.print("Status 0x"); Serial.println(id, HEX);
  autoCalibration();
  
  writeRegister8(DRV2605_REG_RTPIN, 0x00); // no real-time-playback
  
  writeRegister8(DRV2605_REG_WAVESEQ1, 1); // strong click
  writeRegister8(DRV2605_REG_WAVESEQ2, 0);
  
  writeRegister8(DRV2605_REG_SUSTAINPOS, 0);
  writeRegister8(DRV2605_REG_SUSTAINNEG, 0);
  writeRegister8(DRV2605_REG_BREAK, 0);
  writeRegister8(DRV2605_REG_AUDIOMAX, 0x64);
  
  // turn off N_ERM_LRA
  uint8_t calVal = readRegister8(DRV2605_REG_FEEDBACK) & 0x03;
  writeRegister8(DRV2605_REG_FEEDBACK, (readRegister8(DRV2605_REG_FEEDBACK) & 0x7C) | calVal);
  // turn on ERM_OPEN_LOOP
  writeRegister8(DRV2605_REG_CONTROL3, readRegister8(DRV2605_REG_CONTROL3) | 0x20);
  return true;
}

boolean Adafruit_DRV2605::pwmPassthrough() {
  Wire.begin();

  autoCalibration();

  uint8_t calVal = readRegister8(DRV2605_REG_FEEDBACK) & 0x03;
  writeRegister8(DRV2605_REG_FEEDBACK, (readRegister8(DRV2605_REG_FEEDBACK) & 0x7C) );

  setMode(DRV2605_MODE_PWMANALOG);
  writeRegister8(DRV2605_REG_CONTROL3, 0x00);
  return true;
}

void Adafruit_DRV2605::autoCalibration() {
  Serial.println("Starting Auto-Calibration...\n");

  writeRegister8(DRV2605_REG_MODE, 0x00); // out of standby

  writeRegister8(DRV2605_REG_RATEDV, 0xFF); // 5V Rated Voltage

  writeRegister8(DRV2605_REG_CLAMPV, 0xFF); // 5.5V Overdrive Voltage

  writeRegister8(DRV2605_REG_FEEDBACK, 0xB6);

  writeRegister8(DRV2605_REG_CONTROL1, 0x13);

  writeRegister8(DRV2605_REG_CONTROL2, 0xF5);

  writeRegister8(DRV2605_REG_CONTROL3, 0x80);

  setMode(1); // calibrate

  go();

  uint8_t diag_results = (readRegister8(DRV2605_REG_STATUS) & 0x08);

  volatile uint8_t goBit = (readRegister8(DRV2605_REG_GO) & 0x01);
  while (goBit != 0)
  {
    goBit = (readRegister8(DRV2605_REG_GO) & 0x01);
  }

  if (diag_results == 0x00)
  {
    Serial.println("Auto-Calibration was successful: ");
  }
  else
  {
    Serial.println("Auto-Calibration Failed.");
    return;
  }

  //Save these values to EEPROM reloading at boot cycle or repeat auto-cal

  uint8_t ACalComp = readRegister8(DRV2605_REG_AUTOCALCOMP);
  uint8_t ACalBEMF = readRegister8(DRV2605_REG_AUTOCALEMP);
  uint8_t BEMFGain = readRegister8(DRV2605_REG_FEEDBACK & 0x03);

  Serial.print("ACalComp"); 
  Serial.print(": 0x"); Serial.println(ACalComp, HEX);  

  Serial.print("ACalBEMF"); 
  Serial.print(": 0x"); Serial.println(ACalBEMF, HEX);  
  Serial.print("BEMFGain"); 
  Serial.print(": 0x"); Serial.println(BEMFGain, HEX);  
  Serial.println();
}

void Adafruit_DRV2605::setWaveform(uint8_t slot, uint8_t w) {
  writeRegister8(DRV2605_REG_WAVESEQ1+slot, w);
}

void Adafruit_DRV2605::selectLibrary(uint8_t lib) {
  writeRegister8(DRV2605_REG_LIBRARY, lib);
}

void Adafruit_DRV2605::go() {
  writeRegister8(DRV2605_REG_GO, 1);
}

void Adafruit_DRV2605::stop() {
  writeRegister8(DRV2605_REG_GO, 0);
}

void Adafruit_DRV2605::setMode(uint8_t mode) {
  writeRegister8(DRV2605_REG_MODE, mode);
}

void Adafruit_DRV2605::setRealtimeValue(uint8_t rtp) {
  writeRegister8(DRV2605_REG_RTPIN, rtp);
}

/********************************************************************/

uint8_t Adafruit_DRV2605::readRegister8(uint8_t reg) {
  uint8_t x ;
   // use i2c
    Wire.beginTransmission(DRV2605_ADDR);
    Wire.write((byte)reg);
    Wire.endTransmission();
    Wire.requestFrom((byte)DRV2605_ADDR, (byte)1);
    x = Wire.read();

   // Serial.print("$"); Serial.print(reg, HEX); 
   // Serial.print(": 0x"); Serial.println(x, HEX);
  
  return x;
}

void Adafruit_DRV2605::writeRegister8(uint8_t reg, uint8_t val) {
   // use i2c
    Wire.beginTransmission(DRV2605_ADDR);
    Wire.write((byte)reg);
    Wire.write((byte)val);
    Wire.endTransmission();
}

/****************/


// Allow users to use ERM motor or LRA motors

void Adafruit_DRV2605::useERM ()
{
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) & 0x7F);
}

void Adafruit_DRV2605::useLRA ()
{
  writeRegister8(DRV2605_REG_FEEDBACK, readRegister8(DRV2605_REG_FEEDBACK) | 0x80);
}
