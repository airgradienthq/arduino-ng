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

#include "../bsp/BoardDef.h"
#include <Arduino.h>
#include <Wire.h>

class Display {
public:
  const uint16_t COLOR_WHILTE = 1;
  const uint16_t COLOR_BLACK = 0;
#if defined(ESP8266)
  void begin(TwoWire &wire, Stream &debugStream);
#else
#endif
  Display(BoardType type);
  void begin(TwoWire &wire);

  void clear(void); // .clear
  void invertDisplay(uint8_t i);
  void show(); // .show()

  void setContrast(uint8_t value);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void setTextSize(int size);
  void setCursor(int16_t x, int16_t y);
  void setTextColor(uint16_t color);
  void setTextColor(uint16_t foreGroundColor, uint16_t backGroundColor);
  void setText(String text);
  void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w,
                  int16_t h, uint16_t color);

private:
  BoardType _boardType;
  const BoardDef *_bsp = nullptr;
  void *oled;
  bool _isInit = false;
#if defined(ESP8266)
  const char *TAG = "oled";
  Stream *_debugStream = nullptr;
#else

#endif
  bool checkInit(void);
};

#endif /** _AIR_GRADIENT_OLED_H_ */
