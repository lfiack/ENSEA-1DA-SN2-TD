/*
 * sgtl5000.c
 *
 *  Created on: Feb 27, 2024
 *      Author: laurentf
 */

#include "sgtl5000.h"

#include <stdio.h>
#include "main.h"
#include "i2c.h"

#define SGTL5000_DEV_ADDR 0x14

#define SGTL5000_ADDR_CHIP_ID 0x0000
// Comment configurer les autres registres

void sgtl5000_init(void)
{
	printf("sgtl5000_init()\r\n");
	uint8_t pData[2];
	HAL_I2C_Mem_Read(&hi2c2, SGTL5000_DEV_ADDR, SGTL5000_ADDR_CHIP_ID, I2C_MEMADD_SIZE_16BIT, pData, 2, HAL_MAX_DELAY);

	printf("0x%02x 0x%02x\r\n", pData[0], pData[1]);
}
