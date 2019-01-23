/*
 * interface.h
 *
 *  Created on: Jan 23, 2019
 *      Author: quinsy
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "keyb.h"

typedef enum {
	main_menu,
	alarm_menu,
	timer_menu,
	problem_menu,
	settings_menu,
	time_menu,
} menu;

void on_number(int x);
void on_choice(button x);
int16_t complex(int16_t expected, int16_t input, int16_t x);

#endif /* INTERFACE_H_ */
