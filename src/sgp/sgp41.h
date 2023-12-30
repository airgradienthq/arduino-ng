/**
 * @file sgp4x.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient library for SGP4x from Sensirion, this lib is cover
 * libaray publish by Sensirion https://github.com/Sensirion/arduino-i2c-sgp40
 * @version 0.1
 * @date 2023-Dec-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _AIR_GRADIENT_SGP4X_H_
#define _AIR_GRADIENT_SGP4X_H_

#include "../bsp/BoardDef.h"
#include <Arduino.h>
#include <Wire.h>

class Sgp41 {
public:
  Sgp41(BoardType type);
  bool begin(TwoWire &wire);
#if defined(ESP8266)
  bool begin(TwoWire &wire, Stream &stream);
#endif
  void end(void);
  int getTvocIndex(void);
  int getNoxIndex(void);

private:
  bool _isInit = false;
  void *_sensor;
  const BoardDef *bsp = nullptr;
  BoardType _boardType;
#if defined(ESP8266)
  Stream *_debugStream = nullptr;
  const char *TAG = "SGP4x";
#else
#endif
  bool checkInit(void);
  bool boardSupported(void);
  int sdaPin(void);
  int sclPin(void);
  bool getRawSignal(uint16_t &raw_voc, uint16_t &raw_nox,
                    uint16_t defaultRh = 0x8000, uint16_t defaultT = 0x6666);
  bool turnHeaterOff(void);
  bool getSerialNumber(uint16_t serialNumbers[], uint8_t serialNumberSize);
};

#endif /** _AIR_GRADIENT_SGP4X_H_ */
