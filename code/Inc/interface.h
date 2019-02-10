/*
 * interface.h
 *
 *  Created on: Jan 23, 2019
 *      Author: quinsy
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "keyb.h"

// menu 
typedef enum {
	main_menu,
	alarm_menu,
	timer_menu,
	problem_menu,
	settings_menu,
	time_menu,
	queue_menu
} menu;

// timer activation handler
void on_timer();
// alarm activation handler
void on_alarm();

// show display 1 info
void display1();
// show display 2 info
void display2();

// number button press handler
void on_number(int x);
// non-number button press handler 
void on_choice(button x);
// reset input, input index, go to main menu
void done();

// complex numbers input function
int16_t complex(int16_t expected, int16_t input, int16_t x);

#endif /* INTERFACE_H_ */
