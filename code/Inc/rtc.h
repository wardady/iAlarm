/*
 * rtc.h
 *
 *  Created on: Dec 27, 2018
 *      Author: quinsy
 */

#ifndef RTC_H_
#define RTC_H_

#include <stdio.h>

// transform decimal into binary decimal
uint8_t fromDEC(uint8_t dec); 
// transform binary decimal into decimal
uint8_t toDEC(uint8_t binar);

// read aTxBuffer
void get_buffer();

// set time
void set_time(uint8_t sec, uint8_t min, uint8_t hour, uint8_t day, uint8_t date, uint8_t month, uint8_t year);
// set alarm (alarm 2 on ds3231)
void set_alarm(uint8_t min, uint8_t hour, uint8_t day);
// set timer (alarm 1 on ds3231) as alarm with current + given time
void set_timer(uint8_t sec, uint8_t min);

// set alarm 1 to play every second
void alarm1_every_second();
// set alarm 2 to play every minute
void alarm2_every_minute();

// reset alarm (alarm 2 on ds3231)
void reset_alarm();
// reset timer (alarm 1 on ds3231)
void reset_timer();

// reset alarm 1 flag
void reset_flag1();
// reset alarm 2 flag
void reset_flag2();
#endif /* RTC_H_ */
