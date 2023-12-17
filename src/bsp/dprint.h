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

#if defined(ESP8266)
#define AirPrintf(c, ...)                                                   \
  if (this->_debugStream != nullptr)                                        \
  {                                                                         \
    this->_debugStream->printf("[%s] " c "\r\n", this->TAG, ##__VA_ARGS__); \
  }
#else
#include <esp32-hal-log.h>
#define AirPrintf(c, ...) \
  log_i(c, ##__VA_ARGS__)
#endif

#endif /** _AIR_GRADIENT_DEBUG_PRINT_H_ */
