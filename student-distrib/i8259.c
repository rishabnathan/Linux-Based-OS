/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* i8259_init
 *
 *      DESCRIPTION: Initialize the 8259 PIC
 *      INPUTS: none
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: i8259 PIC initiliazed and ready to use
 */    
void i8259_init(void) {
    master_mask = 0xFF; //setting masks to mask all interrupts
    slave_mask = 0xFF;

    outb(ICW1, MASTER_8259_PORT); //master
    outb(ICW1, SLAVE_8259_PORT); // slave


    outb(ICW2_MASTER, MASTER_8259_PORT2); //master
    outb(ICW2_SLAVE, SLAVE_8259_PORT2); //slave 


    outb(ICW3_MASTER, MASTER_8259_PORT2); //master
    outb(ICW3_SLAVE, SLAVE_8259_PORT2); //slave

    outb(ICW4, MASTER_8259_PORT2); //master
    outb(ICW4, SLAVE_8259_PORT2); //slave

    // slave pic enabled
    enable_irq(SLAVE_IRQ);
}

/* enable_irq
 *
 *      DESCRIPTION: enables (unmask) the specified IRQ
 *      INPUTS: irq_num, irq number for slave or master pic
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: unmasks irq num on slave or master
 */    
void enable_irq(uint32_t irq_num) {

    // dont do anything if irq number is out of range
    if((irq_num < 0) || (irq_num > 15)){ // 0 -15 is range for all irq nums
        return;
    }

    // setting mask to 11111110 initially and will left shift accoridng to irq num
    uint8_t mask = 0xFE; 

    // for master pic
    if((irq_num >= 0) && (irq_num <= 7)){ // 0 - 7 is range for master pic
        int b;
        
        // left shifting to the irq num
        for(b = 0; b < irq_num; b++){
            mask <<= 1;
            mask++;
        }

        //setting master mask to allow irq interrupt
        master_mask &= mask;
        outb(master_mask, MASTER_8259_PORT2);
        return;
    } 
    
    // for slave pic
    if((irq_num >= 8) && (irq_num <= 15)){ // 8 - 15 is range for slave pic


        //setting mask for slave pic
        irq_num -= 8; //subracting 8 gives us number of times to left shift
        int b;
        for(b = 0; b < irq_num; b++){
            mask <<= 1;
            mask++;
        }

        //setting slave mask to allow irq interrupt
        slave_mask &= mask;
        outb(slave_mask, SLAVE_8259_PORT2);
        return;
    }
    




}
/* disable_irq
 *
 *      DESCRIPTION: Disable (mask) the specified IRQ
 *      INPUTS: irq_num, irq number for slave or master pic
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: masks irq num on slave or master
 */     
void disable_irq(uint32_t irq_num) {

    // if irq number invalid, do nothing
    if((irq_num < 0) || (irq_num > 15)){
        return;
    }

    uint8_t mask = 0x01;

    //setting mask for master pic
    if((irq_num >= 0) && (irq_num <= 7)){
        int b;

        for(b = 0; b < irq_num; b++){
            mask <<= 1;
            
        }

        //setting master mask to disable irq interrupt
        master_mask |= mask;
        outb(master_mask, MASTER_8259_PORT2);
        return;
    } 
    
    if((irq_num >= 8) && (irq_num <= 15)){

        //setting mask for slave pic to see which irq to disable
        irq_num -= 8;
        int b;
        for(b = 0; b < irq_num; b++){
            mask <<= 1;
            
        }
        //setting slave mask to disable irq interrupt
        slave_mask |= mask;
        outb(slave_mask, SLAVE_8259_PORT2);
        return;
    }
}

/* send_eoi
 *
 *      DESCRIPTION: Send end-of-interrupt signal for the specified IRQ
 *      INPUTS: irq_num, irq number for slave or master pic
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: sends eoi signal for irq
 */     
void send_eoi(uint32_t irq_num) {

    // master pic
    if((irq_num >= 0) && (irq_num <= 7)){ //0-7 is range for master
        outb(EOI | irq_num, MASTER_8259_PORT);
    }

    // slave pic
    if((irq_num >= 8) && (irq_num <= 15)){ //8-15 is range for slave
        outb(EOI | (irq_num - 8), SLAVE_8259_PORT);
        outb(EOI + SLAVE_IRQ, MASTER_8259_PORT);
    }
}
