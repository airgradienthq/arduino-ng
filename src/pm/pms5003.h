/**
 * @file pms5003.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient Plantower PMS5003 sensor this library is reference
 *        from https://github.com/fu-hsi/pms
 * @version 0.1
 * @date 2023-Dec-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _AIR_GRADIENT_PMS5003_H_
#define _AIR_GRADIENT_PMS5003_H_

#include "Stream.h"
#include "../bsp/AirGradientBsp.h"

class PMS5003
{
public:
  static const uint16_t SINGLE_RESPONSE_TIME = 1000;
  static const uint16_t TOTAL_RESPONSE_TIME  = 1000 * 10;
  static const uint16_t STEADY_RESPONSE_TIME = 1000 * 30;
  static const uint16_t BAUD_RATE            = 9600;

  struct DATA
  {
    // Standard Particles, CF=1
    uint16_t PM_SP_UG_1_0;
    uint16_t PM_SP_UG_2_5;
    uint16_t PM_SP_UG_10_0;

    // Atmospheric environment
    uint16_t PM_AE_UG_1_0;
    uint16_t PM_AE_UG_2_5;
    uint16_t PM_AE_UG_10_0;
  };

#if defined(ESP8266)
  PMS5003(AirGradientBoardType_t def);
  bool begin(Stream* _debugStream);
#else
  PMS5003(AirGradientBoardType_t def, HardwareSerial& serial);
#endif
  bool begin(void);
  void sleep();
  void wakeUp();
  void activeMode();
  void passiveMode();

  void requestRead();
  bool read(DATA &data);
  bool readUntil(DATA &data, uint16_t timeout = SINGLE_RESPONSE_TIME);
  int pm25ToAQI(int pm02);

private:
  enum STATUS
  {
    STATUS_WAITING,
    STATUS_OK
  };
  enum MODE
  {
    MODE_ACTIVE,
    MODE_PASSIVE
  };

  uint8_t _payload[12];
  Stream *_stream;
  DATA *_data;
  STATUS _status;
  MODE _mode = MODE_ACTIVE;

  uint8_t _index = 0;
  uint16_t _frameLen;
  uint16_t _checksum;
  uint16_t _calculatedChecksum;

  bool _isInit = false;
  bool _isSleep = false;

  AirGradientBoardType_t _boardDef;
  const AirGradientBsp_t *bsp;
#if defined(ESP8266)
  Stream *_debugStream;
  const char *TAG = "PMS5003";
#else
  HardwareSerial& _serial;
#endif
  bool _readUntil(DATA &data, uint16_t timeout);
  bool checkInit(void);

  void loop();
};
#endif /** _AIR_GRADIENT_PMS5003_H_ */
