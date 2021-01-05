/*
 * terminal.h --> Header file for terminal.c
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "syscalls.h"

typedef struct terminal_struct{
    uint8_t term_id;
    int8_t pid;
    int8_t running;
    uint32_t screen_x;
    uint32_t screen_y;
    uint8_t *video_mem;
    volatile uint8_t buf[128];
    volatile uint8_t bufferIndex;
    volatile uint8_t E_flag; // flag to keep track of whether enter has been hit

} terminal_t;

volatile uint8_t active_terminal_id;
uint8_t curr_terminal_id;
terminal_t terminals[MAX_TERMINALS];

int32_t start_terminal(uint8_t terminal_id);
int32_t save_terminal(uint8_t terminal_id);
int32_t restore_terminal(uint8_t terminal_id);
int32_t switch_terminals(uint8_t old_terminal_id, uint8_t new_terminal_id);

/* initialize the terminal */
//void initialize_terminal();

/* open the terminal */
int32_t terminal_open(const uint8_t* filename);

/* close the terminal */
int32_t terminal_close(int32_t fd);

/* read data from keyboard buffer after enter has been hit or max buffer size has been reached */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);

/* display data to the terminal */
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);


#endif
