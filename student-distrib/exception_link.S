#define ASM 1
#include "exception_link.h"


.globl keyboard_wrapper, rtc_wrapper, pit_wrapper

# void rtc_wrapper()
# Saves registers/flags and calls the rtc_handler function
# Inputs   : None
# Outputs  : None
# Registers: Standard C calling convention
rtc_wrapper:
    pushal #save all registers
    pushfl #saves all flags
    call irq_handler_rtc
    popfl #restores all registers and flags
    popal
    iret

# void keyboard_wrapper()
# Saves registers/flags and calls the keyboard_handler function
# Inputs   : None
# Outputs  : None
# Registers: Standard C calling convention
keyboard_wrapper:
    pushal
    pushfl
    call keyboard_handler
    popfl
    popal
    iret

# void pit_wrapper()
# Saves registers/flags and calls the pit_handler function
# Inputs   : None
# Outputs  : None
# Registers: Standard C calling convention
pit_wrapper:
    pushal
    pushfl
    call pit_handler
    popfl
    popal
    iret
.end
