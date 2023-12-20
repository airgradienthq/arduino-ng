/**
 * @file led.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient LED, this library will be cover the library Adafruit_NeoPixel
 *        https://github.com/adafruit/Adafruit_NeoPixel
 * @version 0.1
 * @date 2023-Dec-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _AIR_GRADIENT_LED_H_
#define _AIR_GRADIENT_LED_H_

#include <Arduino.h>
#include "AirGradientBsp.h"
#include "../library/Adafruit_NeoPixel/Adafruit_NeoPixel.h"

class AirGradientLED
{
public:
#if defined(ESP8266)
  AirGradientLED(AirGradientBoardType_t type, Stream& debugStream);
#else
#endif
  AirGradientLED(AirGradientBoardType_t type);
  void begin(void);
  void ledOn(void);
  void ledOff(void);
  void ledToggle(void);
  void ledSetColor(uint8_t red, uint8_t green, uint8_t blue, int ledNum);
  void ledSetBrighness(uint8_t brightness);
  int getNumberOfLed(void);
private:
  uint8_t redColor;
  uint8_t greenColor;
  uint8_t blueColor;

  const AirGradientBsp_t* _bsp;
  bool _isInit = false;
  uint8_t _ledState = 0;
  AirGradientBoardType_t _boardType;
  Adafruit_NeoPixel* pixels = nullptr;
#if defined(ESP8266)
  Stream *_debugStream = NULL;
  const char* TAG = "LED";
#else
#endif
  bool checkInit(void);
  void ledSingleOn(void);
  void ledSingleOff(void);
  void ledSingleToggle(void);
};

#endif /** _AIR_GRADIENT_LED_H_ */
