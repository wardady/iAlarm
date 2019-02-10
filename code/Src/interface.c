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

// string arrays for displays
const char *days[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const char *d[7] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
const char *mon[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char *diff[3] = {"EASY", "MEDIUM", "HARD"};

// timer activation handler
void on_timer()
{
	// checks whether timer was set and ds3231 flag
	if (tim && (aTxBuffer[15] & 0b1)) {
		reset_flag1();
		// turn on buzzer
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 0);
		// flag that lets timer buzzer to be turned off
		tim = 2;
	}
}

// alarm activation handler
void on_alarm()
{
	// checks whether queue is not empty and ds3231 flag
	if (q->size && (aTxBuffer[15] & 0b10)) {
		reset_flag2();
		// set next alarm into ds3231
		alarm al = next(q);
		set_alarm(al.min, al.hour, al.dotw);
		// flag that lets alarm buzzer to be turned off
		alrm = 1;
		// turn on buzzer
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 0);
		// get problem
		get_problem();
		cmenu = problem_menu; hint = 0;
	}
}

// show display 1 info
void display1()
{
	LCD5110_clear(&lcd1);
	// show current time and date
	LCD5110_printf(&lcd1, BLACK, "---iAlarm----\n   %02d:%02d:%02d\n    %s\n %2d %s 20%02d\n", hour, min, sec, days[day-1 % 7], date, mon[month-1 % 12], year);
	// print "TIMER" if timer is not ser
	if (tim == 0) LCD5110_print("TIMER   ", BLACK, &lcd1);
	// print time to timer activation if timer is set
	else if (tim == 1) {
		uint8_t sec_left = tim_sec - sec, min_left = tim_min - min;
		if (sec_left > 59) {
			sec_left += 60;
			min_left--;
		}
		if (min_left > 59) min_left += 60;
		LCD5110_printf(&lcd1, BLACK, "%02d:%02d   ", min_left, sec_left);
	// print "TIMER" on black background if timer buzzer is ready to be shut down
	} else if (tim == 2) {
		LCD5110_print("TIMER", WHITE, &lcd1);
		LCD5110_print("   ", BLACK, &lcd1);
	}
	// print "TIMER" on black background if alarm queue is not empty, else - on normal one
	if (q->size) LCD5110_print("ALARM\n", WHITE, &lcd1);
	else LCD5110_print("ALARM\n", BLACK, &lcd1);
}

// show display 2 info
void display2()
{
	LCD5110_clear(&lcd2);
	// show main menu
	if (cmenu == main_menu)
		LCD5110_print("A > ALARM\nB > TIMER\nC > PROBLEM\nD > SETTINGS\n# > ALRM LIST", BLACK, &lcd2);
	// show alarm set menu with black background on current input
	else if (cmenu == alarm_menu) {
		if (i == 0) {
			LCD5110_printf(&lcd2, BLACK, "--SET ALARM--\n  HH:MM DoW R\n  ");
			LCD5110_printf(&lcd2, WHITE, "%02d", input[0]);
			LCD5110_printf(&lcd2, BLACK, ":%02d %s %d\nA>NEXT B>PREV\nC>BACK D>DONE\n", input[1], d[input[2] % 7], input[3]);
		} else if (i == 1) {
			LCD5110_printf(&lcd2, BLACK, "--SET ALARM--\n  HH:MM DoW R\n  %02d:", input[0]);
			LCD5110_printf(&lcd2, WHITE, "%02d", input[1]);
			LCD5110_printf(&lcd2, BLACK, " %s %d\nA>NEXT B>PREV\nC>BACK D>DONE\n", d[input[2] % 7], input[3]);
		} else if (i == 2) {
			LCD5110_printf(&lcd2, BLACK, "--SET ALARM--\n  HH:MM DoW R\n  %02d:%02d ", input[0], input[1]);
			LCD5110_printf(&lcd2, WHITE, "%s", d[input[2] % 7]);
			LCD5110_printf(&lcd2, BLACK, " %d\nA>NEXT B>PREV\nC>BACK D>DONE\n", input[3]);
		} else if (i == 3) {
			LCD5110_printf(&lcd2, BLACK, "--SET ALARM--\n  HH:MM DoW R\n  %02d:%02d %s ", input[0], input[1], d[input[2] % 7]);
			LCD5110_printf(&lcd2, WHITE, "%d", input[3]);
			LCD5110_printf(&lcd2, BLACK, "\nA>NEXT B>PREV\nC>BACK D>DONE\n");
		}
	}
	// show timer set menu with black background on current input
	else if (cmenu == timer_menu) {
		if (i == 0) {
			LCD5110_printf(&lcd2, BLACK, "--SET TIMER--\n     MM:SS\n     ");
			LCD5110_printf(&lcd2, WHITE, "%02d", input[0]);
			LCD5110_printf(&lcd2, BLACK, ":%02d\nA>NEXT B>PREV\nC>BACK D>DONE\n", input[1]);
		} else if (i == 1) {
			LCD5110_printf(&lcd2, BLACK, "--SET TIMER--\n     MM:SS\n     %02d:", input[0]);
			LCD5110_printf(&lcd2, WHITE, "%02d", input[1]);
			LCD5110_printf(&lcd2, BLACK, "\nA>NEXT B>PREV\nC>BACK D>DONE\n");
		}
	}
	// show settings menu
	else if (cmenu == settings_menu)
		LCD5110_printf(&lcd2, BLACK, "A > TIME\nB > LIGHTS\nC > PROBLEM\n     %s\nD > SAVE\n", diff[difficulty]);
	// show time set menu with black background on current input
	else if (cmenu == time_menu) {
		if (i == 0) {
			LCD5110_printf(&lcd2, BLACK, "  ");
			LCD5110_printf(&lcd2, WHITE, "%02d", input[0]);
			LCD5110_printf(&lcd2, BLACK, ":%02d:%02d\n    %s\n %2d %s 20%02d\nA>NEXT B>PREV\nC>BACK D>DONE\n", input[1], input[2], days[input[3] % 7], input[4], mon[input[5] % 12], input[6]);
		} else if (i == 1) {
			LCD5110_printf(&lcd2, BLACK, "  %02d:", input[0]);
			LCD5110_printf(&lcd2, WHITE, "%02d", input[1]);
			LCD5110_printf(&lcd2, BLACK, ":%02d\n    %s\n %2d %s 20%02d\nA>NEXT B>PREV\nC>BACK D>DONE\n", input[2], days[input[3] % 7], input[4], mon[input[5] % 12], input[6]);
		} else if (i == 2) {
			LCD5110_printf(&lcd2, BLACK, "  %02d:%02d:", input[0], input[1]);
			LCD5110_printf(&lcd2, WHITE, "%02d", input[2]);
			LCD5110_printf(&lcd2, BLACK, "\n    %s\n %2d %s 20%02d\nA>NEXT B>PREV\nC>BACK D>DONE\n", days[input[3] % 7], input[4], mon[input[5] % 12], input[6]);
		} else if (i == 3) {
			LCD5110_printf(&lcd2, BLACK, "  %02d:%02d:%02d\n    ", input[0], input[1], input[2]);
			LCD5110_printf(&lcd2, WHITE, "%s", days[input[3] % 7]);
			LCD5110_printf(&lcd2, BLACK, "\n %2d %s 20%02d\nA>NEXT B>PREV\nC>BACK D>DONE\n", input[4], mon[input[5] % 12], input[6]);
		} else if (i == 4) {
			LCD5110_printf(&lcd2, BLACK, "  %02d:%02d:%02d\n    %s\n ", input[0], input[1], input[2], days[input[3] % 7]);
			LCD5110_printf(&lcd2, WHITE, "%2d", input[4]);
			LCD5110_printf(&lcd2, BLACK, " %s 20%02d\nA>NEXT B>PREV\nC>BACK D>DONE\n", mon[input[5] % 12], input[6]);
		} else if (i == 5) {
			LCD5110_printf(&lcd2, BLACK, "  %02d:%02d:%02d\n    %s\n %2d ", input[0], input[1], input[2], days[input[3] % 7], input[4]);
			LCD5110_printf(&lcd2, WHITE, "%s", mon[input[5] % 12]);
			LCD5110_printf(&lcd2, BLACK, " 20%02d\nA>NEXT B>PREV\nC>BACK D>DONE\n", input[6]);
		} else if (i == 6) {
			LCD5110_printf(&lcd2, BLACK, "  %02d:%02d:%02d\n    %s\n %2d %s ", input[0], input[1], input[2], days[input[3] % 7], input[4], mon[input[5] % 12]);
			LCD5110_printf(&lcd2, WHITE, "20%02d", input[6]);
			LCD5110_printf(&lcd2, BLACK, "\nA>NEXT B>PREV\nC>BACK D>DONE\n");
		}
	}
	// show problem menu with cheater indication
	else if (cmenu == problem_menu) {
		// different outputs for different difficulties
		if (difficulty == 0) {
			if (hint) LCD5110_printf(&lcd2, BLACK, "***CHEATER***\n%13d\n%13d\n*******%5d*\n%13d\n", num1, num2, enum1, unum1);
			else LCD5110_printf(&lcd2, BLACK, "-----ADD-----\n%13d\n%13d\n-------------\n%13d\n", num1, num2, unum1);
		} else if (difficulty == 1) {
			if (hint) LCD5110_printf(&lcd2, BLACK, "***CHEATER***\n%13d\n%13d\n*******%5d*\n%13d\n", num1, num2, enum1, unum1);
			else LCD5110_printf(&lcd2, BLACK, "--MULTIPLY---\n%13d\n%13d\n-------------\n%13d\n", num1, num2, unum1);
		// show black background on current input
		} else if (difficulty == 2) {
			if (hint) LCD5110_printf(&lcd2, BLACK, "***CHEATER***\n%6d + %3di\n%6d + %3di\n %4d + %4di*\n", num1, num2, num3, num4, enum1, enum2);
			else LCD5110_printf(&lcd2, BLACK, "--MULTIPLY---\n%6d + %3di\n%6d + %3di\n-------------\n", num1, num2, num3, num4);
			if (i == 0) {
				LCD5110_printf(&lcd2, BLACK, " ");
				LCD5110_printf(&lcd2, WHITE, "%4d", unum1);
				LCD5110_printf(&lcd2, BLACK, " + %4di\n", unum2);
			} else if (i == 1) {
				LCD5110_printf(&lcd2, BLACK, " %4d + ", unum1);
				LCD5110_printf(&lcd2, WHITE, "%4d", unum2);
				LCD5110_printf(&lcd2, BLACK, "i\n");
			}
		}
	}
	// show alarm queue
	else if (cmenu == queue_menu) {
		if (!q->size) LCD5110_printf(&lcd2, BLACK, "\n     empty\n \nA>SET NEW\n#>DONE\n");
		else {
			LCD5110_printf(&lcd2, BLACK, "  ALARM %d/%d\n  %02d:%02d %s %d\nA>NEXT B>PREV\nC>DEL  D>CLR\n#>DONE\n", i+1, q->size, q->list[i].hour, q->list[i].min, d[q->list[i].dotw], q->list[i].repeat);
		}
	}
}

// number button press handler
void on_number(int x)
{
	// different actions for different menus and inputs
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

// non-number button press handler
void on_choice(button x)
{
	// main menu
	if (cmenu == main_menu) {
		// go to alarm set menu
		if (x == button_a) {
			input[3] = 1;
			cmenu = alarm_menu;
		}
		// go to timer set menu
		else if (x == button_b) cmenu = timer_menu;
		// get random problem and go to problem menu
		else if (x == button_c) {
			get_problem();
			cmenu = problem_menu;
		}
		// go to settings menu
		else if (x == button_d) cmenu = settings_menu;
		// go to alarm queue menu
		else if (x == button_hash) cmenu = queue_menu;
	// alarm set menu
	} else if (cmenu == alarm_menu) {
		// next input
		if (x == button_a) i = (i + 1) % 4;
		// previous input
		else if (x == button_b) {
			if (i == 0) i = 3;
			else i--;
		// cancel and go to main menu
		} else if (x == button_c) done();
		// set alarm and go to main menu
		else if (x == button_d) {
			if (!insert(q, input[1], input[0], input[2], input[3]))
				set_alarm(q->list[0].min, q->list[0].hour, q->list[0].dotw);
			done();
		}
	// timer set menu
	} else if (cmenu == timer_menu) {
		// next/previous input
		if ((x == button_a) || (x == button_b)) i = !i;
		// cancel and go to main menu
		else if (x == button_c) done();
		// set timer and go to main menu
		else if (x == button_d) {
			set_timer(input[1], input[0]);
			tim = 1;
			done();
		}
	// problem menu
	} else if (cmenu == problem_menu) {
		// next/previous input for hard problems
		if ((difficulty == 2) && ((x == button_a) || (x == button_b))) i = !i;
		// allow to cancel if problem menu is called by user, not alarm
		else if (!alrm && (x == button_c)) done();
		// check user answer
		else if (x == button_d) {
			// correct answer == shut down buzzer and go to main menu
			if ((enum1 == unum1) && (enum2 == unum2)) {
				done();
				alrm = 0;
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 1);
			// incorrect answer == get next problem
			} else get_problem();
		// cheat code input
		} else if (x == button_hash) {
			if (hash) hash--;
			else if (!hash) {
				hint = 1;
				hash = 4;
			}
		}
	// settings menu
	} else if (cmenu == settings_menu) {
		// go to time set menu
		if (x == button_a) cmenu = time_menu;
		// turn off/on display light
		else if (x == button_b) HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_15);
		// change difficulty
		else if (x == button_c) difficulty = (difficulty + 1) % 3;
		// go to main menu
		else if (x == button_d) cmenu = main_menu;
	// time set menu
	} else if (cmenu == time_menu) {
		// next input
		if (x == button_a) i = (i + 1) % 7;
		// previous input
		else if (x == button_b) {
			if (i == 0) i = 6;
			else i--;
		// cancel and go to main menu
		} else if (x == button_c) done();
		// set time and go to main menu
		else if (x == button_d) {
			set_time(input[2], input[1], input[0], input[3], input[4], input[5], input[6]);
			done();
		}
	// alarm queue menu
	} else if (cmenu == queue_menu) {
		if (x == button_a) {
			// set new alarm if queue is empty
			if (!q->size) {
				i = 0;
				input[3] = 1;
				cmenu = alarm_menu;
			// show next alarm
			} else {
				if (i < q->size - 1) i++;
				else i = 0;
			}
		// show previous alarm
		} else if (x == button_b) {
			if (i > 0) i--;
			else i = q->size - 1;
		// remove current alarm
		} else if (x == button_c) {
			if (i == 0) {
				if (q->size > 1) {
					q->list[0].repeat = 0;
					alarm al = next(q);
					set_alarm(al.min, al.hour, al.dotw);
				} else {
					reset_alarm();
					clear_queue(q);
				}
			} else {
				for (uint8_t j = i; j < q->size - 1; j++)
					q->list[j] = q->list[j + 1];
				q->size--;
				i--;
			}
		// clear queue
		} else if (x == button_d) {
			reset_alarm();
			clear_queue(q);
		// go to main menu
		} else if (x == button_hash) {
			done();
		}
	}
	// allow star button to shut down timer buzzer (if it is not alarm buzzer) 
	if ((tim == 2) && (!alrm) && (x == button_star)) {
		tim = 0;
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, 1);
	}
}

// reset input, input index, go to main menu
void done()
{
	cmenu = main_menu;
	i = 0;
	for (uint8_t j = 0; j < 7; j++) input[j] = 0;
}

// compex numbers input function
int16_t complex(int16_t expected, int16_t input, int16_t x)
{
	if (expected < 0)
		return -1 * ((abs(input) * 10 + x) % 1000);
	else
		return (input * 10 + x) % 1000;
}
