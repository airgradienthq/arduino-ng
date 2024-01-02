#include "LedBar.h"

#include "../library/Adafruit_NeoPixel/Adafruit_NeoPixel.h"

#define pixel() ((Adafruit_NeoPixel *)this->pixels)

#if defined(ESP8266)
void LedBar::begin(Stream &debugStream) {
  this->_debugStream = &debugStream;
  this->begin();
}
#else
#endif
LedBar::LedBar(BoardType type) : _boardType(type) {}

/**
 * @brief LED bar initialize
 *
 */
void LedBar::begin(void) {
  if (this->_isInit) {
    return;
  }

  /** Get board support package define */
  this->_bsp = getBoardDef(this->_boardType);
  if ((this->_bsp == nullptr) || (this->_bsp->LED.supported == false) ||
      (this->_bsp->LED.rgbNum == 0)) {
    AgLog("Board Not supported or LED not available on board");
    return;
  }

  /** Init pixels */
  this->pixels = new Adafruit_NeoPixel(
      this->_bsp->LED.rgbNum, this->_bsp->LED.pin, NEO_GRB + NEO_KHZ800);
  pixel()->begin();
  pixel()->clear();

  /** Init LED status array */
  this->leds = new Led[this->_bsp->LED.rgbNum];
  for (int i = 0; i < this->_bsp->LED.rgbNum; i++) {
    this->leds[i].blue = 0;
    this->leds[i].green = 0;
    this->leds[i].red = 0;
    this->leds[i].status = 0; /** OFF */
  }

  this->_isInit = true;

  AgLog("Init");
}

/**
 * @brief Turn on all LED , must set color (@ref setColor) before setOn. This
 * method turn on LED follow lastest LED color
 */
void LedBar::setOn(void) {
  if (this->checkInit() == false) {
    return;
  }
  this->_setOn();
  pixel()->show();
}

/**
 * @brief Turn on special led ON, must set color (@ref setColor) before turn on.
 * Method turn on LED follow lastest LED color
 *
 * @param ledNum Index of LED from 0 to getNumberOfLed() - 1
 */
void LedBar::setOn(int ledNum) {
  if (this->ledNumInvalid(ledNum)) {
    return;
  }
  this->_setOn(ledNum);
  pixel()->show();
}

/**
 * @brief Turn off all led bar
 */
void LedBar::setOff(void) {
  if (this->checkInit() == false) {
    return;
  }
  this->_setOff();
  pixel()->show();
}

/**
 * @brief Turn off special LED on bar
 *
 * @param ledNum Index of LED from 0 to getNumberOfLed() - 1
 */
void LedBar::setOff(int ledNum) {
  if (this->ledNumInvalid(ledNum)) {
    return;
  }
  this->_setOff(ledNum);
  pixel()->show();
}

/**
 * @brief Toggle all LED bar
 */
void LedBar::setToggle(void) {
  if (this->checkInit() == false) {
    return;
  }
  this->_setToggle();
  pixel()->show();
}

/**
 * @brief Toggle special LED on bar
 *
 * @param ledNum Index of LED from 0 to getNumberOfLed() - 1
 */
void LedBar::setToggle(int ledNum) {
  if (this->ledNumInvalid(ledNum)) {
    return;
  }
  this->_setToggle(ledNum);
  pixel()->show();
}

/**
 * @brief Set LED color, if LED is on the color update immedietly. Otherwise
 * must setOn to show LED color
 *
 * @param red Color Red (0 - 255)
 * @param green Color Green (0 - 255)
 * @param blue Color Blue (0 - 255)
 * @param ledNum Index of LED from 0 to getNumberOfLed() - 1
 */
void LedBar::setColor(uint8_t red, uint8_t green, uint8_t blue, int ledNum) {
  if (this->ledNumInvalid(ledNum)) {
    return;
  }

  this->leds[ledNum].red = red;
  this->leds[ledNum].green = green;
  this->leds[ledNum].blue = blue;

  if (this->_isLedOn(ledNum)) {
    pixel()->setPixelColor(ledNum, red, green, blue);
    pixel()->show();
  }
}

/**
 * @brief Set LED brightness apply for all LED bar
 *
 * @param brightness Brightness (0 - 255)
 */
void LedBar::setBrighness(uint8_t brightness) {
  if (this->checkInit() == false) {
    return;
  }
  pixel()->setBrightness(brightness);
  pixel()->show();
}

/**
 * @brief Get number of LED on bar
 *
 * @return int Number of LED
 */
int LedBar::getNumberOfLed(void) {
  if (this->checkInit() == false) {
    return 0;
  }

  return this->_bsp->LED.rgbNum;
}

bool LedBar::checkInit(void) {
  if (this->_isInit) {
    return true;
  }
  AgLog("LED is not initialized");
  return false;
}

bool LedBar::ledNumInvalid(int ledNum) {
  if (this->checkInit() == false) {
    return true;
  }

  if ((ledNum < 0) || (ledNum >= this->_bsp->LED.rgbNum)) {
    AgLog("ledNum invalid: %d", ledNum);
    return true;
  }
  return false;
}

uint8_t LedBar::_isLedOn(int ledNum) { return this->leds[ledNum].status; }

void LedBar::_setOff(int ledNum) {
  this->leds[ledNum].status = 0;
  pixel()->setPixelColor(ledNum, 0, 0, 0);

  AgLog("LED %d Off", ledNum);
}

void LedBar::_setOff(void) {
  for (int ledNum = 0; ledNum < this->_bsp->LED.rgbNum; ledNum++) {
    this->_setOff(ledNum);
  }
}

void LedBar::_setOn(int ledNum) {
  this->leds[ledNum].status = 1;
  pixel()->setPixelColor(ledNum, this->leds[ledNum].red,
                         this->leds[ledNum].green, this->leds[ledNum].blue);
  AgLog("LED %d On", ledNum);
}

void LedBar::_setOn(void) {
  for (int ledNum = 0; ledNum < this->_bsp->LED.rgbNum; ledNum++) {
    this->_setOn(ledNum);
  }
}

void LedBar::_setToggle(void) {
  for (int ledNum = 0; ledNum < this->_bsp->LED.rgbNum; ledNum++) {
    this->_setToggle(ledNum);
  }
}

void LedBar::_setToggle(int ledNum) {
  if (this->_isLedOn(ledNum)) {
    this->_setOff(ledNum);
  } else {
    this->_setOn(ledNum);
  }
}
