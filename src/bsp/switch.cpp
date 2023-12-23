/**
 * @file switch.cpp
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient Switch
 * @version 0.1
 * @date 2023-Dec-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "switch.h"

#if defined(ESP8266)
AirGradientSwitch::AirGradientSwitch(AirGradientBoardType_t type,
                                     Stream &debugStream) : _debugStream(&debugStream),
                                                            _boardType(type)
{
}
#else
#endif

AirGradientSwitch::AirGradientSwitch(AirGradientBoardType_t type) : _boardType(type)
{
}

void AirGradientSwitch::begin(void)
{
  if (this->_isInit)
  {
    return;
  }

  this->_bsp = AirGradientBspGet(this->_boardType);
  if ((this->_bsp == nullptr) || (this->_bsp->SW.supported == false))
  {
    AgLog("Board not supported or switch not available");
    return;
  }

  if(this->_boardType == BOARD_DIY_PRO_INDOOR_V4_2)
  {
    pinMode(this->_bsp->SW.pin, INPUT_PULLUP);
  }
  else 
  {
    pinMode(this->_bsp->SW.pin, INPUT);
  }

  this->_isInit = true;
  AgLog("Init");
}

AirGradientSwitch::State AirGradientSwitch::getState(void)
{
  if (this->checkInit() == false)
  {
    return AirGradientSwitch::UnKnown;
  }

  if(digitalRead(this->_bsp->SW.pin) == this->_bsp->SW.activeLevel)
  {
    return AirGradientSwitch::Presssed;
  }
  return AirGradientSwitch::Released;
}

bool AirGradientSwitch::checkInit(void)
{
  if (this->_isInit)
  {
    return true;
  }
  AgLog("Switch not initialized");
  return false;
}
