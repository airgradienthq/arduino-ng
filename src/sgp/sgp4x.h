/**
 * @file sgp4x.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient library for SGP4x from Sensirion, this lib is covert libaray
 *        publish by Sensirion https://github.com/Sensirion/arduino-i2c-sgp40
 * @version 0.1
 * @date 2023-Dec-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _AIR_GRADIENT_SGP4X_H_
#define _AIR_GRADIENT_SGP4X_H_

#include <Arduino.h>
#include <Wire.h>
#include "../bsp/AirGradientBsp.h"

class AirGradientSgp4x
{
public:
#if defined(ESP8266)
  AirGradientSgp4x(AirGradientBoardType_t type, Stream& stream);
#else
#endif
  AirGradientSgp4x(AirGradientBoardType_t type);
  bool begin(TwoWire& wire);
  void end(void);
  bool boardSupported(void);
  int sdaPin(void);
  int sclPin(void);
  bool getRawSignal(uint16_t& raw, uint16_t defaultRh = 0x8000, uint16_t defaultT = 0x6666);
  bool turnHeaterOff(void);
  bool getSerialNumber(uint16_t serialNumbers[], uint8_t serialNumberSize);
private:
  bool _isInit = false;
  void* _sensor;
  // SensirionI2CSgp40 _sensor;
  const AirGradientBsp_t* bsp = nullptr;
  AirGradientBoardType_t _boardType;
#if defined(ESP8266)
  Stream* _debugStream = nullptr;
  const char* TAG = "SGP4x";
#else
#endif
  bool checkInit(void);
};

#endif /** _AIR_GRADIENT_SGP4X_H_ */
