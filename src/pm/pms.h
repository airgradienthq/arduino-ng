/**
 * @file pms.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief Plantower sensor PMS2.5
 * @version 0.1
 * @date 2023-Dec-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef _AIR_GRADIENT_PMS_H_
#define _AIR_GRADIENT_PMS_H_

#include "Arduino.h"
#include "../bsp/board_defs.h"

class PMS2_5
{
public:
  struct Data
  {
    // Standard Particles, CF=1
    uint16_t PM_SP_UG_1_0;
    uint16_t PM_SP_UG_2_5;
    uint16_t PM_SP_UG_10_0;

    // Atmospheric environment
    uint16_t PM_AE_UG_1_0;
    uint16_t PM_AE_UG_2_5;
    uint16_t PM_AE_UG_10_0;

    // Raw particles count (number of particles in 0.1l of air
    uint16_t PM_RAW_0_3;
    uint16_t PM_RAW_0_5;
    uint16_t PM_RAW_1_0;
    uint16_t PM_RAW_2_5;
    uint16_t PM_RAW_5_0;
    uint16_t PM_RAW_10_0;

    // Formaldehyde (HCHO) concentration in mg/m^3 - PMSxxxxST units only
    uint16_t AMB_HCHO;

    // Temperature & humidity - PMSxxxxST units only
    int16_t PM_TMP;
    uint16_t PM_HUM;
  };

  PMS2_5(AirGradientBoardDef_t def, Stream *_debugStream);
  PMS2_5(AirGradientBoardDef_t def);
  bool begin(void);
  bool begin(Stream* _debugStream);
  void end(void);
  void sleep(void);
  void wakeUp(void);
  void activeMode(void);
  void passiveMode(void);
  bool isReady(void);
  bool read(Data& data);

  static const uint16_t SINGLE_RESPONSE_TIME = 1000;
  static const uint16_t TOTAL_RESPONSE_TIME = 1000 * 10;
  static const uint16_t STEADY_RESPONSE_TIME = 1000 * 30;

private:
  const char* TAG = "PMS2.5";
  Stream* _debugStream;
  AirGradientBoardDef_t _boardDef;
  bool _isInit = false;
  bool _isReady = false;
  uint32_t _lastInitTime;
  Stream* _uartStream;

  enum Mode { MODE_ACTIVE, MODE_PASSIVE };
  enum Status { STATUS_WAITING, STATUS_OK };
  Mode _mode = Mode::MODE_ACTIVE;
  Data* _data;
  Status _status;
  uint8_t _index = 0;
  uint16_t _frameLen;
  uint16_t _checksum;
  uint16_t _calculatedChecksum;
  uint8_t _payload[32];

  char Char_PM1[10];
  char Char_PM2[10];
  char Char_PM10[10];

  /** Functions */
  bool init(const AirGradientBSP_t* bsp);
  bool init(int txPin, int rxPin);
  bool init(int txPin, int rxPin, uint32_t baud);
  bool isInit(void);

  void _requestRead(void);
  bool _getRaw(Data& data);
  int _getPM2_Raw(void);
  int _getPM1_Raw(void);
  int _getPM10_Raw(void);
  bool _readUntil(Data& data, uint16_t timeout = SINGLE_RESPONSE_TIME);
  void _readLoop(void);
  const char* _getPMS2_5_Str(void);

};

#endif /** _AIR_GRADIENT_PMS_H_ */
