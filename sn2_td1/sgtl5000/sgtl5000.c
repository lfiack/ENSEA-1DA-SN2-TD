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

	sgtl5000_i2c_clear_bit(SGTL5000_ADDR_CHIP_ANA_POWER, (1 << 12) | (1 << 13));
	sgtl5000_i2c_set_bit(SGTL5000_ADDR_CHIP_ANA_POWER, (1 << 5) | (1 << 6));


	/* On peut supprimer à partir d'ici... */
	sgtl5000_i2c_read_register(SGTL5000_ADDR_CHIP_DIG_POWER, &reg);
	printf("DIG_POWER = 0x%04X\r\n", reg);

	sgtl5000_i2c_write_register(SGTL5000_ADDR_CHIP_DIG_POWER, 1);

	sgtl5000_i2c_read_register(SGTL5000_ADDR_CHIP_DIG_POWER, &reg);
	printf("DIG_POWER = 0x%04X\r\n", reg);

	// (1<<4) signifie 1 décalé à gauche de 4 bits (= 0x0010)
	// Bit 4 à 1
	sgtl5000_i2c_set_bit(SGTL5000_ADDR_CHIP_DIG_POWER, (1<<4));	// Modifie le bit 4 sans modifier les autres

	sgtl5000_i2c_read_register(SGTL5000_ADDR_CHIP_DIG_POWER, &reg);
	printf("DIG_POWER = 0x%04X\r\n", reg);

	sgtl5000_i2c_clear_bit(SGTL5000_ADDR_CHIP_DIG_POWER, (1<<4));	// Mets le bit 4 à 0 sans modifier les autres

	sgtl5000_i2c_read_register(SGTL5000_ADDR_CHIP_DIG_POWER, &reg);
	printf("DIG_POWER = 0x%04X\r\n", reg);

	/* ...Jusque là */
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

int sgtl5000_i2c_set_bit(uint16_t reg_address, uint16_t mask)
{
	uint16_t reg;
	// Lire le registre
	sgtl5000_i2c_read_register(reg_address, &reg);

	// Modifier les bits selon le masque
	// Tous les 0 dans le masque ne modifient pas le registre
	// Tous les 1 dans le masque imposent des 1 dans le registre
	// Pour faire ça, on utilise un OU logique (symbole | : altgr+6)
	// Exemple : 0x0001 0010 | 0x0000 0001 = 0x0001 0011
	reg = reg | mask;

	// Écrire le registre
	sgtl5000_i2c_write_register(reg_address, reg);

	return 0;
}

int sgtl5000_i2c_clear_bit(uint16_t reg_address, uint16_t mask)
{
	uint16_t reg;
	// Lire le registre
	sgtl5000_i2c_read_register(reg_address, &reg);

	// Modifier les bits selon le masque
	// Tilde inverse tous les bits (les 0 deviennent des 1, les 1 deviennent des 0)
	// Tilde : altgr+2
	// ~(0x0001 0000) = 0x1110 1111
	reg = reg & (~mask);

	// Écrire le registre
	sgtl5000_i2c_write_register(reg_address, reg);

	return 0;
}
