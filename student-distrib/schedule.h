#ifndef _SCHEDULE_H
#define _SCHEDULE_H

#define PIT_FREQ 1193180
#define COMMAND 0x36
#define PIT_PORTNUM 0x43
#define PIT_CHANNEL 0x40
#define div 100
int32_t term_proc[MAX_TERMINALS]; // Keeps track of the pid being executed for each terminal

void initialize_pit();
void pit_handler();
#endif
