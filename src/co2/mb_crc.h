/**
 * @file mb_crc.h
 * @author Phat N. (phat@pdiytech.com)
 * @brief AirGradient Modbus CRC calculation
 * @version 0.1
 * @date 2023-Dec-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _AIR_GRADIENT_MODBUS_CRC_H_
#define _AIR_GRADIENT_MODBUS_CRC_H_

#include <stdint.h>

uint16_t AgMb16Crc(uint8_t *buf, uint16_t len);

#endif /** _AIR_GRADIENT_MODBUS_CRC_H_ */
