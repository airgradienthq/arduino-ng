/**
 * @file StatusLed.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief Single LED header
 * @version 0.1
 * @date 2023-Dec-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _STATUS_LED_H_
#define _STATUS_LED_H_

#include "BoardDef.h"
#include <Arduino.h>

class StatusLed {
public:
  enum State {
    LED_OFF,
    LED_ON,
  };

  StatusLed(BoardType boardType);
#if defined(ESP8266)
  void begin(Stream &debugStream);
#else
#endif
  void begin(void);
  void setOn(void);
  void setOff(void);
  void setToggle(void);
  State getState(void);
  String toString(StatusLed::State state);

private:
  const BoardDef *bsp = nullptr;
  BoardType boardType;
  bool isInit = false;
  State state;
#if defined(ESP8266)
  Stream *_debugStream;
  const char *TAG = "StatusLed";
#else
#endif

  bool checkInit(void);
};

#endif /** _STATUS_LED_H_ */
