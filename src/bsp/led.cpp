/**
 * @file led.cpp
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient board LED control
 * @version 0.1
 * @date 2023-Dec-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "led.h"

#if defined(ESP8266)
AirGradientLED::AirGradientLED(AirGradientBoardType_t type, Stream &debugStream):
  _debugStream(&debugStream),
  _boardType(type)
{
}
#else
#endif
AirGradientLED::AirGradientLED(AirGradientBoardType_t type):
  _boardType(type)
{
}

void AirGradientLED::begin(void)
{
  if(this->_isInit)
  {
    return;
  }

  this->_bsp = AirGradientBspGet(this->_boardType);
  if((this->_bsp == nullptr) || (this->_bsp->LED.supported == false))
  {
    AgLog("Board Not supported or LED not available on board");
    return;
  }

  if(this->_bsp->LED.rgbSupported == false)
  {
    AgLog("Init single LED");
    pinMode(this->_bsp->LED.pin, OUTPUT);
    this->ledSingleOff();
  }
  else
  {
    this->pixels = new Adafruit_NeoPixel(this->_bsp->LED.rgbNum, this->_bsp->LED.pin, NEO_GRB + NEO_KHZ800);
    this->pixels->begin();
    this->pixels->clear();
  }

  this->_isInit = true;
  AgLog("Init");
}
void AirGradientLED::ledOn(void)
{
  if(this->checkInit() == false)
  {
    return;
  }
  if(this->_bsp->LED.rgbSupported == false)
  {
    this->ledSingleOn();
  }
  else 
  {

  }
}

void AirGradientLED::ledOff(void)
{
  if(this->checkInit() == false)
  {
    return;
  }

  if(this->_bsp->LED.rgbSupported == false)
  {
    this->ledSingleOff();
  }
  else 
  {
    this->pixels->clear();
    this->pixels->show();
  }
}
void AirGradientLED::ledToggle(void)
{
  if(this->checkInit() == false)
  {
    return;
  }

  if(this->_bsp->LED.rgbSupported == false)
  {
    this->ledSingleToggle();
  }
  else 
  {

  }
}
void AirGradientLED::ledSetColor(uint8_t red, uint8_t green, uint8_t blue, int ledNum)
{
  if(this->checkInit() == false)
  {
    return;
  }

  if(this->_bsp->LED.rgbSupported)
  {
    this->pixels->setPixelColor(ledNum, red, green, blue);
    this->pixels->show();
  }
}

void AirGradientLED::ledSetBrighness(uint8_t brightness)
{
  if(this->checkInit() == false)
  {
    return;
  }
  
  if(this->_bsp->LED.rgbSupported)
  {
    this->pixels->setBrightness(brightness);
  }
}

int AirGradientLED::getNumberOfLed(void)
{
  if(this->checkInit() == false)
  {
    return 0;
  }
  if(this->_bsp->LED.supported == false)
  {
    return 1;
  }

  if(this->_bsp->LED.rgbSupported)
  {
    return this->_bsp->LED.rgbNum;
  }
  return 1;
}

bool AirGradientLED::checkInit(void)
{
  if(this->_isInit)
  {
    return true;
  }
  AgLog("LED is not initialized");
  return false;
}

void AirGradientLED::ledSingleOn(void)
{
  this->_ledState = this->_bsp->LED.onState;
  digitalWrite(this->_bsp->LED.pin, this->_ledState);
}

void AirGradientLED::ledSingleOff(void)
{
  this->_ledState = !this->_bsp->LED.onState;
  digitalWrite(this->_bsp->LED.pin, this->_ledState);
}

void AirGradientLED::ledSingleToggle(void)
{
  if (this->_ledState == this->_bsp->LED.onState)
  {
    this->ledSingleOff();
  }
  else
  {
    this->ledSingleOn();
  }
}
