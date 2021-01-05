/*
 * rtc.c --> functions for the Real Time Clock
 */

#include "rtc.h"
#include "i8259.h"
#include "terminal.h"
#include "schedule.h"
#include "syscalls.h"

volatile int rtc_int_flag[3] = {0, 0, 0};

/*
*   Function: initialize_rtc()
*   Description: initializes the ports for the RTC,
*                initializes control register A and B, and sets the default frequency
*   inputs: none
*   outputs: none
*   effects: enables IRQ Line on PIC, sends appropriate values to RTC (according to DataSheet)
*/
void initialize_rtc(){
    disable_irq(RTC_IRQ);

    /* turning on periodic interrupts */

    outb(RTC_STATREG_A, RTC_PortNUM); // select Status Register A, and disable NMI
    char previous = inb(CMOS_PortNUM); // read current value of Register B
    outb(RTC_STATREG_B, RTC_PortNUM); // set index again
    outb(previous | 0x40, CMOS_PortNUM); // write the previous value ORed with 0x40 which will turn on bit 6 of register B

    /* Setting up the virtualization process --> statically set frequency to 512 Hz */
    outb(RTC_STATREG_A, RTC_PortNUM);
    previous = inb(CMOS_PortNUM);
    outb(RTC_STATREG_A, RTC_PortNUM);
    outb(((previous & 0xF0) | MAX_RATE), CMOS_PortNUM);

    enable_irq(RTC_IRQ);
    return;
}

/*
*   Function: irq_handler_rtc()
*   Description: Used to test if the RTC is working
*   input:  none
*   output: none
*   effects: updates video memory and sends an eoi to RTC IRQ Line
*/
void irq_handler_rtc(){
    uint8_t temp_pid;
    pcb_t * temp_pcb;
    cli(); // disable interrupts

    /* decrement the counter for the process running in each of the terminals so that they can run to completion */
    int i;
    for (i = 0; i < MAX_TERMINALS; i++){
        temp_pid = term_proc[i];
        temp_pcb = get_pcb_ptr_num(temp_pid);
        if (temp_pcb->rtc_counter > 0) {
            temp_pcb->rtc_counter--;
        }
    }

    send_eoi(RTC_IRQ); // send end of interrupt to RTC's interrupt line

    /* makes sure that you get another interrupt */
    outb(RTC_STATREG_C, RTC_PortNUM);
    inb(CMOS_PortNUM);

    sti(); // enable interrupts again

    return;
}

/*
*   Function: open_rtc()
*   Description: Opens the RTC
*   input:  none
*   output: none
*   effects: Opens the RTC and sets the default frequency to 2Hz
*/
int32_t open_rtc(const uint8_t* filename){
    /* set default frequency to 2 Hz when you open the RTC */
    uint32_t freq = DEFAULT_RATE;
    write_rtc(0, &freq, sizeof(freq));
    return 0;
}

/*
*   Function: close_rtc()
*   Description: Closes the RTC
*   input:  file descriptor being closed
*   output: none
*   effects: Closes the RTC
*/
int32_t close_rtc(int32_t fd){
    return 0;
}


/*
*   Function: read_rtc()
*   Description: Reads data from the RTC after an interrupt occurs
*   input:  fd --> file descriptor
            buf --> buffer to read
            nbytes --> number of bytes that will be returned
*   output: always should be 0
*   effects:
*/
int32_t read_rtc(int32_t fd, void* buf, int32_t nbytes){
    /* spin while interrupt hasn't occurred */
    uint8_t temp_pid = term_proc[curr_terminal_id];
    pcb_t * temp_pcb = get_pcb_ptr_num(temp_pid);

    temp_pcb->rtc_counter = temp_pcb->rtc_freq; // rest the counter of the process that is being read so we get go through its full duration

    sti();
    while (temp_pcb->rtc_counter > 0); // spin until the desired duration has ended
    rtc_int_flag[curr_terminal_id] = 1;
    cli();

    rtc_int_flag[curr_terminal_id] = 0; // reset interrupt flag
    temp_pcb->rtc_counter = temp_pcb->rtc_freq; // reset counter back to its max
    return 0;
}

/*
*   Function: write_rtc()
*   Description: Accepts a specifying interrupt rate and sets the frequency accordingly
*   input:  fd --> file descriptor
            buf --> buffer to read
            nbytes --> number of bytes that will be returned
*   output: number of bytes written (always 4)
*   effects: Changes the frequency to the one specified in the buf
*/
int32_t write_rtc(int32_t fd, const void* buf, int32_t nbytes){
    if (nbytes != 4 || buf == NULL){ // parameter checking
        return -1;
    }

    set_rtc_freq(*((int32_t*)buf)); // set the RTC to the desired frequency

    return 4;
}

/*
*   Function: set_rtc_freq()
*   Description: Helper function for write_rtc that will set the user defined frequency of the RTC
*   input:  frequency --> the user defined frequency
*   output:
*   effects: Sets the new RTC frequency
*/
void set_rtc_freq(int32_t frequency){
    uint8_t temp_pid = term_proc[curr_terminal_id];
    pcb_t * temp_pcb = get_pcb_ptr_num(temp_pid);

    outb(RTC_STATREG_A, RTC_PortNUM); // set index to reg A and disable NMI
    inb(CMOS_PortNUM); // get initial value of reg A


    if ((frequency & (frequency - 1)) != 0){ // parameter check to see if desired frequency is a power of 2
        return;
    }

    if (frequency > MAX_INTERRUPT_FREQ){ // OS shouldn't allow user programs to generate more than 1024 interrupts per second by default
        frequency = MAX_INTERRUPT_FREQ;
    }

    /* set the process's frequency to its desired factor of the max frequency */
    temp_pcb->rtc_freq = MAX_INTERRUPT_FREQ / frequency;
    temp_pcb->rtc_counter = MAX_INTERRUPT_FREQ / frequency;
}
