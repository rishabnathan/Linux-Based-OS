#include "types.h"
#include "lib.h"
#include "terminal.h"
#include "syscalls.h"
#include "x86_desc.h"
#include "paging.h"
#include "i8259.h"
#include "schedule.h"


/*
*   Function: initialize_pit()
*   Description: Sets up pit and sends the neccessary data to the correct ports
*   input:  none
*   output: none
*   effects: Connects the PIT to our PIC
*/
void initialize_pit(){
    int32_t divisor = PIT_FREQ / div;
    outb(COMMAND, PIT_PORTNUM); //tells pit to generate square wave
    outb(divisor & 0xFF, PIT_CHANNEL); //sets the high and low bits of the divisor
    outb(divisor >> 8, PIT_CHANNEL);
    enable_irq(0); //send interrupt to pit port on PIC
    int i;
    for (i = 0; i < MAX_TERMINALS; i++) { //initalize the term proc array
      term_proc[i] = -1;
    }
    curr_terminal_id = -1;
}

/*
*   Function: pit_handler()
*   Description: Handles scheduling so proccess can run in the backround on other terminals
*   input:  none
*   output: none
*   effects: Allows our proccess not running in the active terminal to continue execution
*/
void pit_handler(){
    send_eoi(0);
    cli();
    pcb_t *current_pcb = get_pcb_ptr();
    pcb_t *next_pcb;
    int32_t temp_ebp, temp_esp;

    //get ebp and esp values
    asm volatile ("mov %%ebp, %%eax \n\
                    mov %%esp, %%ebx \n\
                    "
                : "=a" (temp_ebp), "=b" (temp_esp)
    );

    current_pcb->esp = temp_esp; //set the esp and ebp values for the current process
    current_pcb->ebp = temp_ebp;
    uint8_t * video_mem_buf;
    curr_terminal_id++;
    curr_terminal_id = curr_terminal_id % 3; //round robin implementation to cycle through 3 terminals 
    if (active_terminal_id == curr_terminal_id) {
      video_mem_buf = (uint8_t *) VIDEO; //change video memory buff depending on if we are looking at active terminal or not
    } else {
      video_mem_buf = terminals[curr_terminal_id].video_mem;
    }
    page_dir[_132_MB / _4_MB] = ((uint32_t) (vidmap_table)) | (page_permission);
    vidmap_table[0] = ((uint32_t) video_mem_buf) | page_permission; //map video memory into user space so fish can use
    flush_tlb();
    if(term_proc[curr_terminal_id] == -1){ //initalizes a shell in each of the three terminals
        start_terminal(curr_terminal_id);
        //term_proc[curr_terminal_id] = 1; //why set it to 1 and not current_pcb->pid? - meet
    }
    map_page(_128_MB, (term_proc[curr_terminal_id] *_4_MB) + _8_MB);
    next_pcb = get_pcb_ptr_num(term_proc[curr_terminal_id]); //get the next pcb based on the current terminal the scheduler is on
    tss.ss0 = KERNEL_DS;
    tss.esp0 = _8_MB - _8_KB * (next_pcb->pid) - 4; //perform context switch 
    asm volatile("movl %0, %%ebp \n\
                movl %1, %%esp \n\
                "
                :
                : "r" (next_pcb->ebp), "r" (next_pcb->esp)
                );
    flush_tlb();
    sti();
}
