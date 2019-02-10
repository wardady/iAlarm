/*
 * problem.c
 *
 *  Created on: Jan 23, 2019
 *      Author: quinsy
 */
#include <stdlib.h>
#include <stdio.h>

#include "problem.h"

extern uint8_t difficulty, hint, hash;
/*
 * num - randomly generated number
 * unum - user input
 * enum - expected answer
 */
extern int num1, num2, num3, num4, enum1, enum2, unum1, unum2;

// get random problem according to the difficulty
void get_problem()
{
	hash = 4; hint = 0;
	unum1 = 0; unum2 = 0;
	if (difficulty == 0) get_easy();
	else if (difficulty == 1) get_medium();
	else if (difficulty == 2) get_hard();
}

// get random easy problem (addition)
void get_easy()
{
	num1 = rand() % 10000; num2 = rand() % 10000;
	enum1 = num1 + num2; enum2 = 0;
}

// get random medium problem (multiplication)
void get_medium()
{
	num1 = rand() % 1000; num2 = rand() % 100;
	enum1 = num1 * num2; enum2 = 0;
}

// get random hard problem (complex numbers multiplication)
void get_hard()
{
	num1 = (rand() % 41) - 20; num2 = (rand() % 41) - 20;
	num3 = (rand() % 41) - 20; num4 = (rand() % 41) - 20;
	enum1 = num1*num3 - num2*num4; enum2 = num1*num4 + num2*num3;
}
