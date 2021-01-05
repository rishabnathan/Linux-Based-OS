/*
 * rtc.h --> Header file for rtc.c
 */

#ifndef _RTC_H
#define _RTC_H

#include "types.h"
#include "i8259.h"
#include "lib.h"

#define RTC_PortNUM   0x70 // port to specify register number and disable NMI
#define CMOS_PortNUM  0x71 // data register

#define RTC_IRQ     8 // RTC's interrupt line on PIC

/* registers on RTC */
#define RTC_STATREG_A  0xA
#define RTC_STATREG_B  0xB
#define RTC_STATREG_C  0xC

#define RATE_MASK   0x0F
#define PREV_A_MASK 0xF0
#define DEFAULT_RATE    2
#define MAX_INTERRUPT_FREQ 512
#define MAX_RATE    0x7

/* initializes RTC */
void initialize_rtc(void);

/* tests RTC interrupts */
void irq_handler_rtc(void);

/* opens RTC */
int32_t open_rtc(const uint8_t* filename);

/* closes RTC */
int32_t close_rtc(int32_t fd);

/* reads data from RTC */
int32_t read_rtc(int32_t fd, void* buf, int32_t nbytes);

/* writes data to RTC (new frequency value) */
int32_t write_rtc(int32_t fd, const void* buf, int32_t nbytes);

/* helper function for setting new frequency */
void set_rtc_freq(int32_t frequency);


#endif
