/**
 * @file s8.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief Arigradient libarary for Senseair S8 NDIR CO2 sensor
 * @version 0.1
 * @date 2023-Dec-16
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _CO2_S8_H_
#define _CO2_S8_H_

#include "Arduino.h"
#include "../bsp/board_defs.h"

class CO2_S8
{
public:
#if defined(ESP8266)
  CO2_S8(AirGradientBoardDef_t def, Stream *_debugStream);
#endif
  CO2_S8(AirGradientBoardDef_t def);
#if defined(ESP8266)
  bool begin(void);
  bool begin(Stream *_serialDebug);
#else
  bool begin(HardwareSerial* serial);
#endif
  void end(void);
  bool isReady(void);
  int getRaw(void);
  int getCO2(int samples);

private:
  const char* TAG = "CO2_S8";
  /** Variables */
  Stream *_debugStream;
  AirGradientBoardDef_t _boardDef;
  Stream *_uartStream;
#if defined(ESP32)
  HardwareSerial* _serial;
#endif
  bool _isInit = false;
  bool _isReady = false;  /** Wait for sensor ready after 10s heating up */
  uint32_t _lastInitTime;

  /** Commands */

  /** Functions */
  bool _begin(void);
  bool init(const AirGradientBSP_t* bsp);
  bool init(int txPin, int rxPin);
  bool init(int txPin, int rxPin, uint32_t baud);
  bool isInit(void);
  int _getRaw(void);
};

#endif
