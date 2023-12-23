/**
 * @file sht.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient SHT sensor library this library is cover the library from
 *        Sension SHT4x https://github.com/Sensirion/arduino-i2c-sht4x
 * @version 0.1
 * @date 2023-Dec-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _AIR_GRADIENT_SHT_H_
#define _AIR_GRADIENT_SHT_H_

#include <Arduino.h>
#include <Wire.h>

#include "../bsp/AirGradientBsp.h"

class AirGradientSht
{
public:
#if defined(ESP8266)
  AirGradientSht(AirGradientBoardType_t type, Stream& stream);
  bool begin(TwoWire& wire, Stream& debugStream);
#else
#endif
  AirGradientSht(AirGradientBoardType_t type);
  bool begin(TwoWire& wire);
  void end(void);
  bool boardSupported(void);
  int sdaPin(void);
  int sclPin(void);

  bool measureHighPrecision(float& temperature, float& humidity);
  bool measureMediumPrecision(float& temperature, float& humidity);
  bool measureLowestPrecision(float& temperature, float& humidity);

  bool activateHighestHeaterPowerShort(float& temperature, float& humidity);
  bool activateMediumHeaterPowerLong(float& temperature, float& humidity);
  bool activateLowestHeaterPowerLong(float& temperature, float& humidity);

  bool getSerialNumber(uint32_t& serialNumber);
  bool softReset(void);

private:
  AirGradientBoardType_t _boardType;
  bool _isInit = false;
  void* _sensor;
  const AirGradientBsp_t* _bsp = NULL;
#if defined(ESP8266)
  Stream* _debugStream = nullptr;
  const char* TAG = "SHT4x";
#else
#endif
  bool checkInit(void);
};

#endif /** _AIR_GRADIENT_SHT_H_ */
