#include "exception_handler.h"
#include "lib.h"
#include "syscalls.h"

/* divide_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */    
void divide_handler(){
    printf("Divide Exception \n");
    halt(0);
}

/* debug_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void debug_handler(){
    printf("Debug Exception \n");
    halt(0);
}

/* NMI_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void NMI_handler(){
    printf("Nonmaskable interrupt Exception \n");
    halt(0);
}
/* breakpoint_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void breakpoint_handler(){
    printf("Breakpoint \n");
    halt(0);
}

/* overflow_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void overflow_handler(){
    printf("Overflow Exception \n");
    halt(0);
}

/* bound_range_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void bound_range_handler(){
    printf("Bound range exceeded  \n");
    halt(0);
}

/* inv_opcode_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void inv_opcode_handler(){
    printf("Invalid Opcode Exception \n");
    halt(0);
}

/* dev_not_available_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void dev_not_available_handler(){
    printf("Device not available Exception \n");
    halt(0);
}

/* double_fault_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void double_fault_handler(){
    printf("Double Fault Exception \n");
    halt(0);
}

/* coprocessor_segment_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void coprocessor_segment_handler(){
    printf("Coprocessor Segment Overrun Exception \n");
    halt(0);
}

/* invalid_TSS_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void invalid_TSS_handler(){
    printf("Invalid TSS Exception \n");
    halt(0);
}

/* segment_notpresent_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void segment_notpresent_handler(){
    printf("Segment Not Present Exception \n");
    halt(0);
}

/* stack_fault_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void stack_fault_handler(){
    printf("Stack Fault Exception \n");
    halt(0);
}

/* general_protection_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void general_protection_handler(){
    printf("General Protection Exception \n");
    halt(0);
}

/* page_fault_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void page_fault_handler(){
    printf("Page Fault Exception \n");
    halt(0);
}

/* fpu_floating_point_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void fpu_floating_point_handler(){
    printf("FPU Floating Point Exception \n");
    halt(0);
}

/* alignment_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void alignment_handler(){
    printf("Alignment Exception \n");
    halt(0);
}

/* machine_check_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void machine_check_handler(){
    printf("Machine Check Exception \n");
    halt(0);
}

/* simd_floating_point_handler
 *
 *      DESCRIPTION: Called when the respective exception is called
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Prints the given exception and gets stuck in infinte while loop
 */
void simd_floating_point_handler(){
    printf("SIMD Floating Point Exception \n");
    halt(0);
}

/* system_call_handler
 *
 *      DESCRIPTION: Called when the respective a system call is made
 *      INPUTS: NONE
 *      OUTPUTS: NONE
 *      SIDE EFFECTS: Let's user know that a system call has been made and gets stuck in infinte while loop
 */

