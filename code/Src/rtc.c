/*
 * rtc.c
 *
 *  Created on: Dec 27, 2018
 *      Author: quinsy
 */
#include "rtc.h"
#include "i2c.h"

extern uint8_t aTxBuffer[18];

uint8_t fromDEC(uint8_t dec)
{
    return (dec / 10) << 4 | dec % 10;
}

uint8_t toDEC(uint8_t binar)
{
    return ((binar >> 4) * 10) + (binar & 0xF);
}

void set_time(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t date, uint8_t month, uint8_t year)
{
	aTxBuffer[0]=0x00;
	aTxBuffer[1]=fromDEC(sec);
	aTxBuffer[2]=fromDEC(min);
	aTxBuffer[3]=fromDEC(hour);
	aTxBuffer[4]=fromDEC(day);
	aTxBuffer[5]=fromDEC(date);
	aTxBuffer[6]=fromDEC(month);
	aTxBuffer[7]=fromDEC(year);

	aTxBuffer[15] = 0;

	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 18);
}

void get_time()
{
	aTxBuffer[0] = 0;
	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
	I2C_ReadBuffer(hi2c1, (uint16_t) 0xD0, 18);
}

void alarm_30_sec()
{
	get_time();

	aTxBuffer[8] = toDEC(aTxBuffer[0]) + 30;
	aTxBuffer[9] = toDEC(aTxBuffer[1]) + (aTxBuffer[8] / 60);
	aTxBuffer[10] = toDEC(aTxBuffer[2]) + (aTxBuffer[9] / 60);
	aTxBuffer[11] = toDEC(aTxBuffer[3]) + (aTxBuffer[10] / 24);

	aTxBuffer[8] = fromDEC(aTxBuffer[8] % 60);
	aTxBuffer[9] = fromDEC(aTxBuffer[9] % 60);
	aTxBuffer[10] = fromDEC(aTxBuffer[10] % 24);
	aTxBuffer[11] = fromDEC(aTxBuffer[11] % 31);

	aTxBuffer[6] = aTxBuffer[5];
	aTxBuffer[5] = aTxBuffer[4];
	aTxBuffer[4] = aTxBuffer[3];
	aTxBuffer[3] = aTxBuffer[2];
	aTxBuffer[2] = aTxBuffer[1];
	aTxBuffer[1] = aTxBuffer[0];

	aTxBuffer[0] = 0;

	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 11);
}
