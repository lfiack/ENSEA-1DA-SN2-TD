/*
 * sgtl5000.c
 *
 *  Created on: Feb 27, 2024
 *      Author: laurentf
 */

#include "sgtl5000.h"

#include <stdio.h>


void sgtl5000_init(void)
{
	printf("sgtl5000_init()\r\n");
	uint16_t reg;

	sgtl5000_i2c_read_register(SGTL5000_ADDR_CHIP_ID, &reg);
	printf("CHIP_ID = 0x%04X\r\n", reg);

	sgtl5000_i2c_read_register(SGTL5000_ADDR_DIG_POWER, &reg);
	printf("DIG_POWER = 0x%04X\r\n", reg);

	sgtl5000_i2c_write_register(SGTL5000_ADDR_DIG_POWER, 1);

	sgtl5000_i2c_read_register(SGTL5000_ADDR_DIG_POWER, &reg);
	printf("DIG_POWER = 0x%04X\r\n", reg);
}

int sgtl5000_i2c_read_register(uint16_t reg_address, uint16_t * p_data)
{
	HAL_StatusTypeDef ret;
	uint8_t reg[2];
	ret = HAL_I2C_Mem_Read(&hi2c2, SGTL5000_DEV_ADDR, reg_address, I2C_MEMADD_SIZE_16BIT, reg, 2, HAL_MAX_DELAY);

	*p_data = (reg[0] << 8) + reg[1];

	if (ret == HAL_OK)
	{
		// HAL_I2C_Mem_Read s'est bien passé
		return 0;
	}
	else
	{
		// HAL_I2C_Mem_Read a retourné une erreur
		return -1;
	}
}

int sgtl5000_i2c_write_register(uint16_t reg_address, uint16_t data)
{
	HAL_StatusTypeDef ret;
	uint8_t reg[2];

	reg[0] = (data >> 8);
	reg[1] = data & 0xFF;	// Masque (et logique bit à bit entre data et 0x00FF)

	ret = HAL_I2C_Mem_Write(&hi2c2, SGTL5000_DEV_ADDR, reg_address, I2C_MEMADD_SIZE_16BIT, reg, 2, HAL_MAX_DELAY);

	if (ret == HAL_OK)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
