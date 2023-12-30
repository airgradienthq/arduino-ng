/**
 * @file AirGradient.cpp
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient library source
 * @version 0.1
 * @date 2023-Dec-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "AirGradient.h"

AirGradient::AirGradient(BoardType type)
    : pms5003(type), pms5003t(type), s8(type), sht(type), sgp41(type),
      display(type), boardType(type), button(type), statusLed(type),
      ledBar(type), watchdog(type) {}

/**
 * @brief Get pin number for I2C SDA
 *
 * @return int -1 if invalid
 */
int AirGradient::getI2cSdaPin(void) {
  const BoardDef *bsp = getBoardDef(this->boardType);
  if ((bsp == nullptr) || (bsp->I2C.supported == false)) {
    return -1;
  }
  return bsp->I2C.sda_pin;
}

/**
 * @brief Get pin number for I2C SCL
 *
 * @return int -1 if invalid
 */
int AirGradient::getI2cSclPin(void) {
  const BoardDef *bsp = getBoardDef(this->boardType);
  if ((bsp == nullptr) || (bsp->I2C.supported == false)) {
    return -1;
  }
  return bsp->I2C.scl_pin;
}
