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
extern int num1, num2, num3, num4, enum1, enum2, unum1, unum2;

void get_problem()
{
	hash = 4; hint = 0;
	unum1 = 0; unum2 = 0;
	if (difficulty == 0) get_easy();
	else if (difficulty == 1) get_medium();
	else if (difficulty == 2) get_hard();
}

void get_easy()
{
	num1 = rand() % 10000; num2 = rand() % 10000;
	enum1 = num1 + num2;
}

void get_medium()
{
	num1 = rand() % 100; num2 = rand() % 100;
	enum1 = num1 * num2;
}

void get_hard()
{
	num1 = (rand() % 41) - 20; num2 = (rand() % 41) - 20;
	num3 = (rand() % 41) - 20; num4 = (rand() % 41) - 20;
	enum1 = num1*num3 - num2*num4; enum2 = num1*num4 + num2*num3;
}
