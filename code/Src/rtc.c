/*
 * rtc.c
 *
 *  Created on: Dec 27, 2018
 *      Author: quinsy
 */
#include "rtc.h"
#include "i2c.h"

extern uint8_t aTxBuffer[18];

// transform decimal into binary decimal
uint8_t fromDEC(uint8_t dec)
{
    return (dec / 10) << 4 | dec % 10;
}

// transform binary decimal into decimal
uint8_t toDEC(uint8_t binar)
{
    return ((binar >> 4) * 10) + (binar & 0xF);
}

// read aTxBuffer
void get_buffer()
{
	aTxBuffer[0] = 0;
	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
	I2C_ReadBuffer(hi2c1, (uint16_t) 0xD0, 18);
}

// set time
void set_time(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t date, uint8_t month, uint8_t year)
{
	aTxBuffer[0] = 0;
	aTxBuffer[1] = fromDEC(sec % 60);
	aTxBuffer[2]= fromDEC(min % 60);
	aTxBuffer[3] = fromDEC(hour % 24);
	aTxBuffer[4] = fromDEC((day % 7) + 1);
	aTxBuffer[5] = fromDEC(date % 32);
	aTxBuffer[6] = fromDEC((month + 1) % 12);
	aTxBuffer[7] = fromDEC(year % 100);

	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 8);
}

// set alarm (alarm 2 on ds3231)
void set_alarm(uint8_t min, uint8_t hour, uint8_t day)
{
	get_buffer();

	aTxBuffer[12] = fromDEC(min % 60) & 0b01111111;
	aTxBuffer[13] = fromDEC(hour % 24) & 0b01111111;
	aTxBuffer[14] = (fromDEC((day % 7) + 1) & 0b01111111) | 0b01000000;

	for (uint8_t i = 11; i > 0; --i) {
		aTxBuffer[i] = aTxBuffer[i-1];
	}
	aTxBuffer[0] = 0;

	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 15);
}

// set timer (alarm 1 on ds3231) as alarm with current + given time
void set_timer(uint8_t sec, uint8_t min)
{
	get_buffer();

	aTxBuffer[8] = toDEC(aTxBuffer[0]) + (sec % 60);
	aTxBuffer[9] = toDEC(aTxBuffer[1]) + (aTxBuffer[8] / 60) + (min % 60);
	aTxBuffer[10] = toDEC(aTxBuffer[2]) + (aTxBuffer[9] / 60);
	aTxBuffer[11] = toDEC(aTxBuffer[4]) + (aTxBuffer[10] / 24);

	aTxBuffer[8] = fromDEC(aTxBuffer[8] % 60) & 0b01111111;
	aTxBuffer[9] = fromDEC(aTxBuffer[9] % 60) & 0b01111111;
	aTxBuffer[10] = fromDEC(aTxBuffer[10] % 24) & 0b01111111;
	aTxBuffer[11] = fromDEC(aTxBuffer[11] % 31) & 0b01111111;

	for (uint8_t i = 7; i > 0; i--) {
		aTxBuffer[i] = aTxBuffer[i-1];
	}
	aTxBuffer[0] = 0;

	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 12);
}

// set alarm 1 to play every second
void alarm1_every_second()
{
	get_buffer();

	aTxBuffer[8] = 0b10000000;
	aTxBuffer[9] = 0b10000000;
	aTxBuffer[10] = 0b10000000;
	aTxBuffer[11] = 0b10000000;

	for (uint8_t i = 7; i > 0; --i) {
		aTxBuffer[i] = aTxBuffer[i-1];
	}
	aTxBuffer[0] = 0;

	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 12);
}

// set alarm 2 to play every minute
void alarm2_every_minute()
{
	get_buffer();

	aTxBuffer[12] = 0b10000000;
	aTxBuffer[13] = 0b10000000;
	aTxBuffer[14] = 0b10000000;

	for (uint8_t i = 11; i > 0; --i) {
		aTxBuffer[i] = aTxBuffer[i-1];
	}
	aTxBuffer[0] = 0;

	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 15);
}

// reset alarm 1 flag
void reset_flag1()
{
	get_buffer();

	aTxBuffer[16] &= 0b11111110;

	for (uint8_t i = 15; i > 0; --i) {
		aTxBuffer[i] = aTxBuffer[i-1];
	}
	aTxBuffer[0] = 0;

	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 18);
}

// reset alarm 2 flag
void reset_flag2()
{
	get_buffer();

	aTxBuffer[16] &= 0b11111101;

	for (uint8_t i = 15; i > 0; --i) {
		aTxBuffer[i] = aTxBuffer[i-1];
	}
	aTxBuffer[0] = 0;

	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 18);
}

// reset alarm (alarm 2 on ds3231)
void reset_alarm()
{
	get_buffer();

	aTxBuffer[16] &= 0b11111101;

	aTxBuffer[12] = 0;
	aTxBuffer[13] = 0;
	aTxBuffer[14] = 0;

	for (uint8_t i = 11; i > 0; --i) {
		aTxBuffer[i] = aTxBuffer[i-1];
	}
	aTxBuffer[0] = 0;

	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 18);
}

// reset timer (alarm 1 on ds3231)
void reset_timer()
{
	get_buffer();

	aTxBuffer[16] &= 0b11111110;

	aTxBuffer[8] = 0;
	aTxBuffer[9] = 0;
	aTxBuffer[10] = 0;
	aTxBuffer[11] = 0;

	for (uint8_t i = 7; i > 0; --i) {
		aTxBuffer[i] = aTxBuffer[i-1];
	}
	aTxBuffer[0] = 0;

	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 1);
	while (HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY) {}
	I2C_WriteBuffer(hi2c1, (uint16_t) 0xD0, 18);
}
