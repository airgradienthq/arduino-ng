/**
 * @file switch.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient Switch library
 * @version 0.1
 * @date 2023-Dec-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _AIR_GRADIENT_SW_H_
#define _AIR_GRADIENT_SW_H_

#include <Arduino.h>
#include "AirGradientBsp.h"

class AirGradientSwitch
{
public:
#if defined(ESP8266)
  AirGradientSwitch(AirGradientBoardType_t type, Stream& debugStream);
#else
#endif
  AirGradientSwitch(AirGradientBoardType_t type);
  enum State
  {
    Presssed, 
    Released,
    UnKnown,  // Not supported status
  };
  void begin(void);
  State getState(void);
private:
  const AirGradientBsp_t* _bsp;
  bool _isInit = false;
  AirGradientBoardType_t _boardType;
  
#if defined(ESP8266)
  Stream* _debugStream = nullptr;
  const char* TAG = "switch";
#else
#endif
  bool checkInit(void);
};

#endif /** _AIR_GRADIENT_SW_H_ */
