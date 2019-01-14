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

// Input pins - the pins you add the interrupt on
//  	(These should be the last pins in keyboard - front view)
// Output pins - the pins you put output on
// 		(These should be the first 4 pins in keyboard)
// On number - reference to function, that will be called when a numeric key is pressed
// On action - reference to function, that will be called when a non-numeric key is pressed
// last_tick - private innter attribute

typedef struct {
	GPIO_TypeDef* input_pins_port;
	GPIO_TypeDef* output_pins_port;
	uint16_t* input_pins;
	uint16_t* output_pins;
	void (*on_number)(int);
	void (*on_choice)(button);
	uint32_t last_tick;
} keyboard;

// Inits a keyboard from the struct
// All parameter should be set before initing
void keyboard_init(keyboard* board);

// Should be called by user if he wants to check if any input is available
// Does nothing if no input available
// It is recommended to call the function from an interrupt
void keyboard_on_input(keyboard* board);

#endif /* KEYB_H_ */
