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

#define disp(func)                                                             \
  if (this->_boardType == BOARD_DIY_BASIC_KIT) {                               \
    ((Adafruit_SSD1306 *)(this->oled))->func;                                  \
  } else {                                                                     \
    ((Adafruit_SH110X *)(this->oled))->func;                                   \
  }

#if defined(ESP8266)
void Display::begin(TwoWire &wire, Stream &debugStream) {
  this->_debugStream = &debugStream;
  this->begin(wire);
}
#else
#endif

Display::Display(BoardType type) : _boardType(type) {}

void Display::begin(TwoWire &wire) {
  this->_bsp = getBoardDef(this->_boardType);
  if ((this->_bsp == nullptr) || (this->_bsp->I2C.supported == false) ||
      (this->_bsp->OLED.supported == false)) {
    AgLog("Init failed: board not supported");
    return;
  }

  /** Init OLED */
  if (this->_boardType == BOARD_DIY_BASIC_KIT) {
    AgLog("Init Adafruit_SSD1306");
    Adafruit_SSD1306 *_oled = new Adafruit_SSD1306();
    _oled->begin(wire, SSD1306_SWITCHCAPVCC, this->_bsp->OLED.addr);
    this->oled = _oled;
  } else {
    AgLog("Init Adafruit_SH1106G");
    Adafruit_SH1106G *_oled = new Adafruit_SH1106G(
        this->_bsp->OLED.width, this->_bsp->OLED.height, &wire);
    _oled->begin(this->_bsp->OLED.addr, false);
    this->oled = _oled;
  }

  this->_isInit = true;
  disp(clearDisplay());
  AgLog("Init");
}

void Display::clear(void) {
  if (this->checkInit() == false) {
    return;
  }
  disp(clearDisplay());
}

void Display::invertDisplay(uint8_t i) {
  if (this->checkInit() == false) {
    return;
  }
  disp(invertDisplay(i));
}

void Display::show() {
  if (this->checkInit() == false) {
    return;
  }
  disp(display());
}

void Display::setContrast(uint8_t value) {
  if (this->checkInit() == false) {
    return;
  }
  disp(setContrast(value));
}

void Display::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (this->checkInit() == false) {
    return;
  }
  disp(drawPixel(x, y, color));
}

void Display::setTextSize(int size) {
  if (this->checkInit() == false) {
    return;
  }
  disp(setTextSize(size));
}

void Display::setCursor(int16_t x, int16_t y) {
  if (this->checkInit() == false) {
    return;
  }
  disp(setCursor(x, y));
}

/**
 * @brief Set Text Color
 *
 * @param color 0:black, 1: While
 */
void Display::setTextColor(uint16_t color) {
  if (this->checkInit() == false) {
    return;
  }
  disp(setTextColor(color));
}

/**
 * @brief Set text foreground color and background color
 *
 * @param foreGroundColor Text Color (foreground color)
 * @param backGroundColor Text background color
 */
void Display::setTextColor(uint16_t foreGroundColor, uint16_t backGroundColor) {
  if (this->checkInit() == false) {
    return;
  }
  disp(setTextColor(foreGroundColor, backGroundColor));
}

void Display::setText(String text) {
  if (this->checkInit() == false) {
    return;
  }
  disp(print(text));
}

void Display::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                         int16_t w, int16_t h, uint16_t color) {
  if (this->checkInit() == false) {
    return;
  }
  disp(drawBitmap(x, y, bitmap, w, h, color));
}

bool Display::checkInit(void) {
  if (this->_isInit) {
    return true;
  }

  AgLog("OLED is not init");
  return false;
}
