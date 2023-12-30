/**
 * @file pms5003.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient Plantower PMS5003 sensor this library is reference from
 * https://github.com/fu-hsi/pms
 * @version 0.1
 * @date 2023-Dec-17
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _AIR_GRADIENT_PMS5003_H_
#define _AIR_GRADIENT_PMS5003_H_

#include "../bsp/BoardDef.h"
#include "Conplug_PMS5003T.h"
#include "Stream.h"

class PMS5003 {
public:
  PMS5003(BoardType def);
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

private:
  bool _isInit = false;
  BoardType _boardDef;
  const BoardDef *bsp;
#if defined(ESP8266)
  Stream *_debugStream;
  const char *TAG = "PMS5003";
#else
  HardwareSerial *_serial;
#endif
  Conplug_PMS5003T *pms;

  bool begin(void);
  bool checkInit(void);
  int pm25ToAQI(int pm02);
};
#endif /** _AIR_GRADIENT_PMS5003_H_ */
