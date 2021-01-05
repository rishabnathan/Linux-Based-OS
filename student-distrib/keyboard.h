/* 
 * keyboard.h --> Header file for keyboard.c
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"



#define KEYBOARD_IRQ    1
#define KEYBOARD_PORTNUM    0x60
#define NUM_KEYS    120

#define BUFFER_SIZE 128

#define LEFTSHIFTU  0xAA
#define LEFTSHIFTD  0x2A

#define RIGHTSHIFTU 0xB6
#define RIGHTSHIFTD  0x36

#define CTRLU   0x9D
#define CTRLD   0x1D

#define ALTU   0xB8
#define ALTD   0x38

#define CAPS    0x3A

#define BACKSPACE 0x0E
#define ENTER 0x1C

#define F1  0x3B
#define F2  0x3C
#define F3  0x3D

volatile uint8_t *keyboard_buffer;
volatile uint8_t bufferIndex;

void initialize_keyboard(void);

extern void keyboard_handler(void);

void get_char(uint8_t scancode_data);

extern void buffer_clear();

void buffer_insert(uint8_t character);

void enter_handler();

void backspace_handler();



#endif /* _KEYBOARD_H */
