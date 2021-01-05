#include "x86_desc.h"
#include "idt.h"
#include "exception_handler.h"
#include "exception_link.h"
#include "keyboard.h"
#include "rtc.h"

/* init_idt
 *
 *      DESCRIPTION: Initalizes all the entries on the IDT
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Depending on what each entry represents, sets the proper values in the idt structure
 */
void init_idt(){
    lidt(idt_desc_ptr); //load the idt
    int i;
    for(i = 0; i < NUM_VEC; i++){ //loops through the 256 exception spots in the table and initalizes them
        idt[i].seg_selector = KERNEL_CS;
        idt[i].reserved4 = 0;
        if(i < INTEL_RESERVED_VECS){  //first 32 exceptions are reserved by intel and have different reserved3 value
            idt[i].reserved3 = 1;
        }
        else{
            idt[i].reserved3 = 0;
        }
        idt[i].reserved2 =  1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;
        idt[i].reserved0 = 0;
        if(i == SYSTEM_CALL_VEC) //if a system call is being made, the priority is not kernel level
            idt[i].dpl = 3; //descriptor priority level
        else
            idt[i].dpl = 0;
        idt[i].present = 1; //inducates a valid interrupt descriptor
    }
    SET_IDT_ENTRY(idt[0], divide_handler); //assign the proper handlers to the right entry on the IDT
    SET_IDT_ENTRY(idt[1], debug_handler);
    SET_IDT_ENTRY(idt[2], NMI_handler);
    SET_IDT_ENTRY(idt[3], breakpoint_handler);
    SET_IDT_ENTRY(idt[4], overflow_handler);
    SET_IDT_ENTRY(idt[5], bound_range_handler);
    SET_IDT_ENTRY(idt[6], inv_opcode_handler);
    SET_IDT_ENTRY(idt[7], dev_not_available_handler);
    SET_IDT_ENTRY(idt[8], double_fault_handler);
    SET_IDT_ENTRY(idt[9], coprocessor_segment_handler);
    SET_IDT_ENTRY(idt[10], invalid_TSS_handler);
    SET_IDT_ENTRY(idt[11], segment_notpresent_handler);
    SET_IDT_ENTRY(idt[12], stack_fault_handler);
    SET_IDT_ENTRY(idt[13], general_protection_handler);
    SET_IDT_ENTRY(idt[14], page_fault_handler);
    SET_IDT_ENTRY(idt[16], fpu_floating_point_handler);
    SET_IDT_ENTRY(idt[17], alignment_handler);
    SET_IDT_ENTRY(idt[18], machine_check_handler);
    SET_IDT_ENTRY(idt[19], simd_floating_point_handler);
    SET_IDT_ENTRY(idt[SYSTEM_CALL_VEC], system_call_handler);
    SET_IDT_ENTRY(idt[KEYBOARD_VEC], keyboard_wrapper);
    SET_IDT_ENTRY(idt[RTC_VEC], rtc_wrapper);
    SET_IDT_ENTRY(idt[PIT_VEC], pit_wrapper);
}
