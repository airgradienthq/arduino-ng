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

#define disp(func)                             \
  if (this->_boardType == BOARD_DIY_BASIC_KIT) \
  {                                            \
    ((Adafruit_SSD1306 *)(this->oled))->func;  \
  }                                            \
  else                                         \
  {                                            \
    ((Adafruit_SH110X *)(this->oled))->func;   \
  }

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
  if(this->_boardType == BOARD_DIY_BASIC_KIT)
  {
    AgLog("Init Adafruit_SSD1306");
    Adafruit_SSD1306* _oled = new Adafruit_SSD1306();
    _oled->begin(wire, SSD1306_SWITCHCAPVCC, this->_bsp->OLED.addr);
    this->oled = _oled;
  }
  else 
  {
    AgLog("Init Adafruit_SH1106G");
    Adafruit_SH1106G* _oled = new Adafruit_SH1106G(this->_bsp->OLED.width, this->_bsp->OLED.height, &wire);
    _oled->begin(this->_bsp->OLED.addr, false);
    this->oled = _oled;
  }

  this->_isInit = true;
  disp(clearDisplay());
  AgLog("Init");
}

void AirGradientOled::clearDisplay(void)
{
  if(this->checkInit() == false)
  {
    return;
  }
  disp(clearDisplay());
}

void AirGradientOled::invertDisplay(uint8_t i)
{
  if(this->checkInit() == false)
  {
    return;
  }
  disp(invertDisplay(i));
}

void AirGradientOled::display()
{
  if(this->checkInit() == false)
  {
    return;
  }
  disp(display());
}

void AirGradientOled::setContrast(uint8_t value)
{
  if(this->checkInit() == false)
  {
    return;
  }
  disp(setContrast(value));
}

void AirGradientOled::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  if(this->checkInit() == false)
  {
    return;
  }
  disp(drawPixel(x, y, color));
}

void AirGradientOled::setTextSize(int size)
{
  if(this->checkInit() == false)
  {
    return;
  }
  disp(setTextSize(size));
}

void AirGradientOled::setCursor(int16_t x, int16_t y)
{
  if(this->checkInit() == false)
  {
    return;
  }
  disp(setCursor(x, y));
}

/**
 * @brief Set Text Color
 *
 * @param color 0:black, 1: While
 */
void AirGradientOled::setTextColor(uint16_t color)
{
  if(this->checkInit() == false)
  {
    return;
  }
  disp(setTextColor(color));
}

/**
 * @brief Set text color and background
 *
 * @param color Text Color (foreground color)
 * @param bg Text background color
 */
void AirGradientOled::setTextColor(uint16_t color, uint16_t bg)
{
  if(this->checkInit() == false)
  {
    return;
  }
  disp(setTextColor(color, bg));
}

void AirGradientOled::setText(String text)
{
  if(this->checkInit() == false)
  {
    return;
  }
  disp(print(text));
}

void AirGradientOled::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                                 int16_t w, int16_t h, uint16_t color)
{
  if(this->checkInit() == false)
  {
    return;
  }
  disp(drawBitmap(x, y, bitmap, w, h, color));
}

bool AirGradientOled::checkInit(void)
{
  if(this->_isInit)
  {
    return true;
  }

  AgLog("OLED is not init");
  return false;
}
