/**
 * @file board_defs.cpp
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient board define
 * @version 0.1
 * @date 2023-Dec-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "AirGradientBsp.h"
#if defined(ESP32)
#include "esp32-hal-log.h"
#endif

const AirGradientBsp_t bsps[BOARD_DEF_MAX] = 
{
  /** BOARD_DIY_BASIC_KIT */
  [0x00] = 
  {
    .SenseAirS8 = 
    {
      .uart_tx_pin = 2,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = true,
#else
      .supported = false,
#endif
    },
    .PMS5003 = 
    {
      .uart_tx_pin = 14,
      .uart_rx_pin = 12,
#if defined(ESP8266)
      .supported = true,
#else
      .supported = false,
#endif
    },
    .name = "BOARD_DIY_BASIC_KIT",
  },
  /** BOARD_DIY_PRO_INDOOR_V4_2 */
  [0x01] = 
  {
    .SenseAirS8 = 
    {
      .uart_tx_pin = 2,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = true,
#else
      .supported = false,
#endif
    },
    .PMS5003 = 
    {
      .uart_tx_pin = 14,
      .uart_rx_pin = 12,
#if defined(ESP8266)
      .supported = true,
#else
      .supported = false,
#endif
    },
	.name = "BOARD_DIY_PRO_INDOOR_V4_2",
  },
  /** BOARD_ONE_INDOOR_MONITOR_V9_0 */
  [0x02] = 
  {
    .SenseAirS8 = 
    {
      .uart_tx_pin = 1,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = false,
#else
      .supported = true,
#endif
    },
    /** Use UART0 don't use define pin number */
    .PMS5003 = 
    {
      .uart_tx_pin = -1,
      .uart_rx_pin = -1,
#if defined(ESP8266)
      .supported = false,
#else
      .supported = true,
#endif
    },
	.name = "BOARD_ONE_INDOOR_MONITOR_V9_0",
  },
  /** BOARD_OUTDOOR_MONITOR_V1_3 */
  [0x03] = 
  {
    .SenseAirS8 = 
    {
      .uart_tx_pin = 1,
      .uart_rx_pin = 0,
#if defined(ESP8266)
      .supported = false,
#else
      .supported = true,
#endif
    },
    /** Use UART0 don't use define pin number */
    .PMS5003 = 
    {
      .uart_tx_pin = -1,
      .uart_rx_pin = -1,
#if defined(ESP8266)
      .supported = false,
#else
      .supported = true,
#endif
    },
	.name = "BOARD_OUTDOOR_MONITOR_V1_3",
  }
};

/**
 * @brief Get Board Support Package
 * 
 * @param def Board define @ref AirGradientBoardType_t
 * @return const AirGradientBsp_t* 
 */
const AirGradientBsp_t *AirGradientBspGet(AirGradientBoardType_t def)
{
  if (def >= BOARD_DEF_MAX)
  {
    return NULL;
  }
  return &bsps[def];
}

#if defined(ESP8266)
#define bspPrintf(c, ...)                                     \
      if (_debug != nullptr)                                  \
      {                                                       \
            _debug->printf("[BSP] " c "\r\n", ##__VA_ARGS__); \
      }
#else
#define bspPrintf(c, ...)\
  log_i(c, ##__VA_ARGS__)
#endif

/**
 * @brief Print list of support Board and sensor
 *
 * @param _debug Serial debug
 */
void AirGradientBspPrint(Stream *_debug)
{
	if (_debug == NULL)
	{
		return;
	}

	for (int i = 0; i < BOARD_DEF_MAX; i++)
	{
		bspPrintf("Board name: %s", bsps[i].name);
		bspPrintf("\tSensor CO2 S8:");
		bspPrintf("\t\tSupported: %d", bsps[i].SenseAirS8.supported);
		if (bsps[i].SenseAirS8.supported)
		{
			bspPrintf("\t\tUART Tx: %d", bsps[i].SenseAirS8.uart_tx_pin);
			bspPrintf("\t\tUART Rx: %d", bsps[i].SenseAirS8.uart_rx_pin);
		}
		bspPrintf("\tSensor PMS50032.5:");
		bspPrintf("\t\tSupported: %d", bsps[i].PMS5003.supported);
		if (bsps[i].PMS5003.supported)
		{
			bspPrintf("\t\tUART Tx: %d", bsps[i].PMS5003.uart_tx_pin);
			bspPrintf("\t\tUART Rx: %d", bsps[i].PMS5003.uart_rx_pin);
		}
	}
}
