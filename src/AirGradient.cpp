#include "AirGradient.h"

#define AG_LIB_VER      "2.5.0"

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

String AirGradient::getVersion(void) { return AG_LIB_VER; }
