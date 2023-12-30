/**
 * @file led.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient LED, this library will be cover the library
 * Adafruit_NeoPixel https://github.com/adafruit/Adafruit_NeoPixel
 * @version 0.1
 * @date 2023-Dec-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _AIR_GRADIENT_LED_H_
#define _AIR_GRADIENT_LED_H_

#include <Arduino.h>

#include "BoardDef.h"

class LedBar {
public:
#if defined(ESP8266)
  void begin(Stream &debugStream);
#else
#endif
  LedBar(BoardType type);
  void begin(void);
  void setOn(void);
  void setOn(int ledNum);
  void setOff(void);
  void setOff(int ledNum);
  void setToggle(void);
  void setToggle(int ledNum);
  void setColor(uint8_t red, uint8_t green, uint8_t blue, int ledNum);
  void setBrighness(uint8_t brightness);

  int getNumberOfLed(void);

private:
  struct Led {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t status;
  };

  const BoardDef *_bsp;
  bool _isInit = false;
  uint8_t _ledState = 0;
  BoardType _boardType;
  void *pixels = nullptr;
  Led *leds;
#if defined(ESP8266)
  Stream *_debugStream = NULL;
  const char *TAG = "LED";
#else
#endif
  bool checkInit(void);
  bool ledNumInvalid(int ledNum);
  uint8_t _isLedOn(int ledNum);
  void _setOff(int ledNum);
  void _setOff(void);
  void _setOn(int ledNum);
  void _setOn(void);
  void _setToggle(void);
  void _setToggle(int ledNum);
};

#endif /** _AIR_GRADIENT_LED_H_ */
