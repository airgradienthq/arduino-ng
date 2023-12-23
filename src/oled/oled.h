/**
 * @file oled.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient OLEd libaray this is base on library
 * - https://github.com/adafruit/Adafruit_SH110X
 * - https://github.com/adafruit/Adafruit-GFX-Library
 * - https://github.com/stblassitude/Adafruit_SSD1306_Wemos_OLED
 * - https://github.com/adafruit/Adafruit_BusIO
 * @version 0.1
 * @date 2023-Dec-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _AIR_GRADIENT_OLED_H_
#define _AIR_GRADIENT_OLED_H_

#include <Arduino.h>
#include <Wire.h>
#include "../bsp/AirGradientBsp.h"

class AirGradientOled
{
public:
#if defined(ESP8266)
  AirGradientOled(AirGradientBoardType_t type, Stream& debugStream);
  void begin(TwoWire& wire, Stream& debugStream);
#else
#endif
  AirGradientOled(AirGradientBoardType_t type);
  void begin(TwoWire& wire);

  void clearDisplay(void);
  void invertDisplay(uint8_t i);
  void display();

  void setContrast(uint8_t value);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void setTextSize(int size);
  void setCursor(int16_t x, int16_t y);
  void setTextColor(uint16_t color);
  void setTextColor(uint16_t color, uint16_t bg);
  void setText(String text);
  void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                  int16_t w, int16_t h, uint16_t color);

private:
  AirGradientBoardType_t _boardType;
  const AirGradientBsp_t* _bsp = nullptr;
  void* oled;
  bool _isInit = false;
#if defined(ESP8266)
  const char* TAG = "oled";
  Stream* _debugStream = nullptr;
#else

#endif
  bool checkInit(void);
};

#endif /** _AIR_GRADIENT_OLED_H_ */
