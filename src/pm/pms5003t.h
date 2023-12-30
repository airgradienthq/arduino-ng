/**
 * @file pms5003t.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief PMS5003T header
 * @version 0.1
 * @date 2023-Dec-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _PMS5003T_H_
#define _PMS5003T_H_

#include "../bsp/BoardDef.h"
#include "Conplug_PMS5003T.h"
#include "Stream.h"

class PMS5003T {
public:
  PMS5003T(BoardType def);
#if defined(ESP8266)
  bool begin(Stream *_debugStream);
#else
  bool begin(HardwareSerial &serial);
#endif

  bool readData(void);
  int getPm01Ae(void);
  int getPm25Ae(void);
  int getPm10Ae(void);
  int getPm03ParticleCount(void);
  int convertPm25ToUsAqi(int pm25);
  float getTemperature(void);
  float getRelativeHumidity(void);

private:
  bool _isInit = false;
  bool _isSleep = false;

  BoardType _boardDef;
  const BoardDef *bsp;
#if defined(ESP8266)
  Stream *_debugStream;
  const char *TAG = "PMS5003T";
#else
  HardwareSerial *_serial;
#endif

  bool begin(void);
  int pm25ToAQI(int pm02);
  Conplug_PMS5003T *pms;
  bool checkInit(void);
};

#endif /** _PMS5003T_H_ */
