/*
 * keyb.h
 *
 *  Created on: Jan 13, 2019
 *      Author: quinsy
 */

#ifndef KEYB_H_
#define KEYB_H_

// Buttons
typedef enum {
  button_a,
  button_b,
  button_c,
  button_d,
  button_star,
  button_hash
} button;

typedef struct {
	GPIO_TypeDef* input_pins_port;
	GPIO_TypeDef* output_pins_port;
	uint16_t* input_pins;
	uint16_t* output_pins;
	void (*on_number)(int);
	void (*on_choice)(button);
	uint32_t last_tick;
} keyboard;

void keyboard_init(keyboard* board);
void keyboard_on_input(keyboard* board);

#endif /* KEYB_H_ */
