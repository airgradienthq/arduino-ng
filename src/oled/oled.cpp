/**
 * @file oled.cpp
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient OLED library
 * @version 0.1
 * @date 2023-Dec-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "oled.h"
#include "../library/Adafruit_SH110x/Adafruit_SH110X.h"
#include "../library/Adafruit_SSD1306_Wemos_OLED/Adafruit_SSD1306.h"
#if defined(ESP8266)
#define disp() ((Adafruit_SSD1306 *)(this->oled))
#else
#define disp() ((Adafruit_SH110X *)(this->oled))
#endif

Adafruit_SH1106G _disp32 = Adafruit_SH1106G(1, 1, &Wire, -1);
Adafruit_SSD1306 _disp8266 = Adafruit_SSD1306();

#if defined(ESP8266)
AirGradientOled::AirGradientOled(AirGradientBoardType_t type, Stream &debugStream) : _boardType(type),
                                                                                     _debugStream(&debugStream)
{
}

void AirGradientOled::begin(TwoWire &wire, Stream &debugStream)
{
  this->_debugStream = &debugStream;
  this->begin(wire);
}
#else
#endif

AirGradientOled::AirGradientOled(AirGradientBoardType_t type) : _boardType(type)
{
}

void AirGradientOled::begin(TwoWire &wire)
{
  this->_bsp = AirGradientBspGet(this->_boardType);
  if ((this->_bsp == nullptr) || (this->_bsp->I2C.supported == false) ||
      (this->_bsp->OLED.supported == false))
  {
    AgLog("Init failed: board not supported");
    return;
  }

  /** Init OLED */
#if defined(ESP8266)
  this->oled = new Adafruit_SSD1306();
#else
  this->oled = new Adafruit_SH1106G(64, 48, &wire);
#endif
  this->_isInit = true;
  AgLog("Init");
}

void AirGradientOled::clearDisplay(void)
{
  disp()->clearDisplay();
}

void AirGradientOled::invertDisplay(uint8_t i)
{
  disp()->invertDisplay(i);
}

void AirGradientOled::display()
{
  disp()->display();
}

void AirGradientOled::setContrast(uint8_t value)
{
  disp()->setContrast(value);
}

void AirGradientOled::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  disp()->drawPixel(x, y, color);
}

void AirGradientOled::setTextSize(int size)
{
  disp()->setTextSize(size);
}

void AirGradientOled::setCursor(int16_t x, int16_t y)
{
  disp()->setCursor(x, y);
}

/**
 * @brief Set Text Color
 *
 * @param color 0:black, 1: While
 */
void AirGradientOled::setTextColor(uint16_t color)
{
  disp()->setTextColor(color);
}

/**
 * @brief Set text color and background
 *
 * @param color Text Color (foreground color)
 * @param bg Text background color
 */
void AirGradientOled::setTextColor(uint16_t color, uint16_t bg)
{
  disp()->setTextColor(color, bg);
}

void AirGradientOled::setText(String text)
{
  disp()->print(text);
}

void AirGradientOled::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                                 int16_t w, int16_t h, uint16_t color)
{
  disp()->drawBitmap(x, y, bitmap, w, h, color);
}
