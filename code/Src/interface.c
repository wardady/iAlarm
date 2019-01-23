/*
 * interface.c
 *
 *  Created on: Jan 23, 2019
 *      Author: quinsy
 */
#include <stdlib.h>

#include "main.h"
#include "lcd5110.h"
#include "rtc.h"
#include "keyb.h"
#include "queue.h"
#include "interface.h"
#include "keyb.h"
#include "problem.h"

extern menu cmenu;
extern uint8_t i, tim, alrm, difficulty, hint, hash;
extern uint8_t input[7];
extern int enum1, enum2, unum1, unum2;
extern Queue* q;

//called after number is pressed
void on_number(int x)
{
	if (cmenu == alarm_menu) {
		if ((i == 0) || (i == 1)) input[i] = (input[i] * 10 + x) % 100;
		else if ((i == 2) || (i == 3)) input[i] = x;
	} else if (cmenu == timer_menu) {
		input[i] = (input[i] * 10 + x) % 100;
	} else if (cmenu == problem_menu) {
		if (difficulty == 0) {
			unum1 = (unum1 * 10 + x) % 100000;
		} else if (difficulty == 1) {
			unum1 = (unum1 * 10 + x) % 10000;
		} else if (difficulty == 2) {
			if (i == 0) unum1 = complex(enum1, unum1, x);
			else if (i == 1) unum2 = complex(enum2, unum2, x);
		}
	} else if (cmenu == time_menu) {
		if (i == 3) input[i] = x;
		else input[i] = (input[i] * 10 + x) % 100;
	}
}

//called after non-number button is pressed
void on_choice(button x)
{
	if (cmenu == main_menu) {
		if (x == button_a) cmenu = alarm_menu;
		else if (x == button_b) cmenu = timer_menu;
		else if (x == button_c) {
			get_problem();
			cmenu = problem_menu;
		}
		else if (x == button_d) cmenu = settings_menu;
	} else if (cmenu == alarm_menu) {
		if (x == button_a) i = (i + 1) % 4;
		else if (x == button_b) {
			if (i == 0) i = 3;
			else i--;
		} else if (x == button_c) {
			cmenu = main_menu; i =0;
			for (uint8_t j = 0; j < 7; j++) input[j] = 0;
		} else if (x == button_d) {
			if (!insert(q, input[1], input[0], input[2], input[3]))
				set_alarm(q->queue[0].min, q->queue[0].hour, q->queue[0].dotw);
			cmenu = main_menu; i = 0;
			for (uint8_t j = 0; j < 7; j++) input[j] = 0;
		}
	} else if (cmenu == timer_menu) {
		if ((x == button_a) || (x == button_b)) i = !i;
		else if (x == button_c) {
			cmenu = main_menu; i = 0;
			for (uint8_t j = 0; j < 7; j++) input[j] = 0;
		} else if (x == button_d) {
			set_timer(input[1], input[0]);
			cmenu = main_menu; i = 0;
			tim = 1;
			for (uint8_t j = 0; j < 7; j++) input[j] = 0;
		}
	} else if (cmenu == problem_menu) {
		if ((difficulty == 2) && ((x == button_a) || (x == button_b))) i = !i;
		else if (!alrm && (x == button_c)) {
			cmenu = main_menu; i = 0;
			unum1 = 0; unum2 = 0;
		} else if (x == button_d) {
			if (difficulty == 2) {
				if ((enum1 == unum1) && (enum2 == unum2)) {
					cmenu = main_menu; i = 0;
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 1);
				} else get_hard();
			} else {
				if (enum1 == unum1) {
					cmenu = main_menu; i = 0;
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 1);
				} else get_problem();
			}
		} else if (x == button_hash) {
			if (hash) hash--;
			else if (!hash) {
				hint = 1;
				hash = 4;
			}
		}

	} else if (cmenu == settings_menu) {
		if (x == button_a) cmenu = time_menu;
		else if (x == button_b) HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_15);
		else if (x == button_c) difficulty = (difficulty + 1) % 3;
		else if (x == button_d) cmenu = main_menu;
	} else if (cmenu == time_menu) {
		if (x == button_a) i = (i + 1) % 7;
		else if (x == button_b) {
			if (i == 0) i = 6;
			else i--;
		} else if (x == button_c) {
			cmenu = main_menu; i = 0;
			for (uint8_t j = 0; j < 7; j++) input[j] = 0;
		} else if (x == button_d) {
			set_time(input[2], input[1], input[0], input[3], input[4], input[5], input[6]);
			cmenu = main_menu; i = 0;
			for (uint8_t j = 0; j < 7; j++) input[j] = 0;
		}
	}
	if ((tim == 2) && (!alrm) && (x == button_star)) {
		tim = 0;
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 1);
	}
}

int16_t complex(int16_t expected, int16_t input, int16_t x)
{
	if (expected < 0)
		return -1 * ((abs(input) * 10 + x) % 1000);
	else
		return (input * 10 + x) % 1000;
}
