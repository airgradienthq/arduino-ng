/**
 * @file dprint.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient debug print
 * @version 0.1
 * @date 2023-Dec-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef _AIR_GRADIENT_DEBUG_PRINT_H_
#define _AIR_GRADIENT_DEBUG_PRINT_H_

#include "Arduino.h"

#define AirPrintTagDef(name) const char *TAG = #name;

#define AirPrintf(c, ...)                                           \
  if (this->_debugStream != nullptr)                              \
  {                                                               \
    this->_debugStream->printf("[%s] %s\r\n", TAG, c, ##__VA_ARGS__); \
  }

#endif /** _AIR_GRADIENT_DEBUG_PRINT_H_ */
