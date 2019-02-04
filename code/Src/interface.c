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
extern uint8_t sec, min, hour, day, date, month, year, tim_sec, tim_min;
extern uint8_t aTxBuffer[18];
extern LCD5110_display lcd1, lcd2;
extern int num1, num2, num3, num4, enum1, enum2, unum1, unum2;
extern queue* q;

const char *days[7] = {"    Monday", "   Tuesday", "  Wednesday", "  Thursday", "    Friday", "  Saturday", "    Sunday"};
const char *d[7] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
const char *mon[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char *diff[3] = {"EASY", "MEDIUM", "HARD"};

void on_timer()
{
	if (tim && (aTxBuffer[15] & 0b1)) {
		reset_flag1();
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 0);
		tim = 2;
	}
}

void on_alarm()
{
	if (q->size && (aTxBuffer[15] & 0b10)) {
		reset_flag2();
		alarm al = next(q);
		set_alarm(al.min, al.hour, al.dotw);
		alrm = 1;
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 0);
		get_problem();
		cmenu = problem_menu; hint = 0;
	}
}

void display1()
{
	LCD5110_clear(&lcd1);
	LCD5110_printf(&lcd1, BLACK, "---iAlarm---\n   %02d:%02d:%02d\n %s\n % 2d %s 20%02d\n", hour, min, sec, days[day-1 % 7], date, mon[month-1 % 12], year);
	if (tim == 0) LCD5110_print("TIMER   ", BLACK, &lcd1);
	else if (tim == 1) {
		uint8_t sec_left = tim_sec - sec, min_left = tim_min - min;
		if (sec_left > 59) {
			sec_left += 60;
			min_left--;
		}
		if (min_left > 59) min_left += 60;
		LCD5110_printf(&lcd1, BLACK, "%02d:%02d   ", min_left, sec_left);
	} else if (tim == 2) {
		LCD5110_print("TIMER", WHITE, &lcd1);
		LCD5110_print("   ", BLACK, &lcd1);
	}
	if (q->size) LCD5110_print("ALARM\n", WHITE, &lcd1);
	else LCD5110_print("ALARM\n", BLACK, &lcd1);
}

void display2()
{
	LCD5110_clear(&lcd2);
	if (cmenu == main_menu)
		LCD5110_print("---iAlarm---\nA > ALARM\nB > TIMER\nC > PROBLEM\nD > SETTINGS\n", BLACK, &lcd2);
	else if (cmenu == alarm_menu)
		LCD5110_printf(&lcd2, BLACK, "--SET ALARM--\n  HH:MM DoW R\n  %02d:%02d %s %d\nA>NEXT B>PREV\nC>BACK D>DONE\n", input[0], input[1], d[input[2] % 7], input[3]);
	else if (cmenu == timer_menu)
		LCD5110_printf(&lcd2, BLACK, "--SET TIMER--\n     MM:SS\n     %02d:%02d\nA>NEXT B>PREV\nC>BACK D>DONE\n", input[0], input[1]);
	else if (cmenu == settings_menu)
		LCD5110_printf(&lcd2, BLACK, "A > TIME\nB > LIGHTS\nC > PROBLEM\n     %s\nD > SAVE\n", diff[difficulty]);
	else if (cmenu == time_menu)
		LCD5110_printf(&lcd2, BLACK, "   %02d:%02d:%02d\n %s\n % 2d %s 20%02d\nA>NEXT B>PREV\nC>BACK D>DONE\n", input[0], input[1], input[2], days[input[3] % 7], input[4], mon[input[5] % 12], input[6]);
	else if (cmenu == problem_menu) {
		if (difficulty == 0) {
			if (hint) LCD5110_printf(&lcd2, BLACK, "***CHEATER***\n%13d\n%13d\n*******%5d*\n%13d\n", num1, num2, enum1, unum1);
			else LCD5110_printf(&lcd2, BLACK, "-----ADD-----\n%13d\n%13d\n-------------\n%13d\n", num1, num2, unum1);
		} else if (difficulty == 1) {
			if (hint) LCD5110_printf(&lcd2, BLACK, "***CHEATER***\n%13d\n%13d\n*******%5d*\n%13d\n", num1, num2, enum1, unum1);
			else LCD5110_printf(&lcd2, BLACK, "--MULTIPLY---\n%13d\n%13d\n-------------\n%13d\n", num1, num2, unum1);
		} else if (difficulty == 2) {
			if (hint) LCD5110_printf(&lcd2, BLACK, "***CHEATER***\n%6d + %3di\n%6d + %3di\n %4d + %4di*\n %4d + %4di\n", num1, num2, num3, num4, enum1, enum2, unum1, unum2);
			else LCD5110_printf(&lcd2, BLACK, "--MULTIPLY---\n%6d + %3di\n%6d + %3di\n-------------\n %4d + %4di\n", num1, num2, num3, num4, unum1, unum2);
		}
	}
}

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
			unum1 = (unum1 * 10 + x) % 100000;
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
		} else if (x == button_c) done();
		else if (x == button_d) {
			if (!insert(q, input[1], input[0], input[2], input[3]))
				set_alarm(q->list[0].min, q->list[0].hour, q->list[0].dotw);
			done();
		}
	} else if (cmenu == timer_menu) {
		if ((x == button_a) || (x == button_b)) i = !i;
		else if (x == button_c) done();
		else if (x == button_d) {
			set_timer(input[1], input[0]);
			tim = 1;
			done();
		}
	} else if (cmenu == problem_menu) {
		if ((difficulty == 2) && ((x == button_a) || (x == button_b))) i = !i;
		else if (!alrm && (x == button_c)) done();
		else if (x == button_d) {
			if ((enum1 == unum1) && (enum2 == unum2)) {
				done();
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 1);
			} else get_problem();
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
		} else if (x == button_c) done();
		else if (x == button_d) {
			set_time(input[2], input[1], input[0], input[3], input[4], input[5], input[6]);
			done();
		}
	}
	if ((tim == 2) && (!alrm) && (x == button_star)) {
		tim = 0;
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 1);
	}
}

void done()
{
	cmenu = main_menu;
	i = 0;
	for (uint8_t j = 0; j < 7; j++) input[j] = 0;
}

int16_t complex(int16_t expected, int16_t input, int16_t x)
{
	if (expected < 0)
		return -1 * ((abs(input) * 10 + x) % 1000);
	else
		return (input * 10 + x) % 1000;
}
