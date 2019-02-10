///*
// * keyb.c
// *
// *  Created on: Jan 13, 2019
// *      Author: quinsy
// */
#include "stm32f4xx_hal.h"
#include "keyb.h"

// write value to output pins
static void keyboard_pins_write(keyboard* board, int value)
{
	for (int i = 0; i < 4; i++)
		HAL_GPIO_WritePin(board->output_pins_port, board->output_pins[i], value);
}

// call corresponding button handler
static void keyboard_on_press(keyboard* board, int col_pin, int row_pin)
{
	void (*on_number)(int) = board->on_number;
	void (*on_choice)(button) = board->on_choice;

	if (col_pin < 3 && row_pin < 3)
		on_number(1 + row_pin * 3 + col_pin);
	else if (col_pin == 1 && row_pin == 3)
		on_number(0);
	else if (col_pin == 3)
		on_choice(button_a + row_pin);
	else if (col_pin == 0 && row_pin == 3)
		on_choice(button_star);
	else if (col_pin == 2 && row_pin == 3)
		on_choice(button_hash);
}

// initialize keayboard
void keyboard_init(keyboard* board)
{
	keyboard_pins_write(board, 1);
	board->last_tick = HAL_GetTick();
}

// read input from keyboard
void keyboard_on_input(keyboard* board)
{
	uint32_t current_tick = HAL_GetTick();

	// if statement to fix contact bounce
	if (current_tick - board->last_tick > 333) {
		int col_pin = -1;
		int row_pin = -1;

		// find pressed button's column by reading interrupt pins
		for (int input_pin = 0; input_pin < 4; ++input_pin) {
			if (HAL_GPIO_ReadPin(board->input_pins_port, board->input_pins[input_pin])) {
				col_pin = input_pin;
				break;
			}
		}

		keyboard_pins_write(board, 0);

		// find pressed button's row by enabling output pins one by one
		if (col_pin != -1) {
			for (int output_pin = 0; output_pin < 4; ++output_pin) {
				HAL_GPIO_WritePin(board->output_pins_port, board->output_pins[output_pin], 1);
				if (HAL_GPIO_ReadPin(board->input_pins_port, board->input_pins[col_pin])) {
					row_pin = output_pin;
					break;
				}
			}
		}

		// cal corresponding button handler
		if (row_pin != -1 && col_pin != -1)
			keyboard_on_press(board, col_pin, row_pin);

		board->last_tick = current_tick;
		keyboard_pins_write(board, 1);
	}
}
