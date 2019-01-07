/*
 * rtc.h
 *
 *  Created on: Dec 27, 2018
 *      Author: quinsy
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdio.h>

uint8_t fromDEC(uint8_t dec);
uint8_t toDEC(uint8_t binar);
void get_buffer();
void set_time(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t date, uint8_t month, uint8_t year);
void alarm1_timer(uint8_t sec, uint8_t min, uint8_t hour, uint8_t date);
void alarm2_timer(uint8_t min, uint8_t hour, uint8_t date);
void alarm1_every_second();
void alarm2_every_minute();
#endif /* RTC_H_ */
