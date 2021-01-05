#define ASM 1
#include "system_call_link.h"

.globl system_call_handler, halt, execute, read, write, open, close

# void system_call_handler()
# Saves registers/flags and calls the rtc_handler function
# Inputs   : None
# Outputs  : None
# Registers: Standard C calling convention

system_call_handler:
    cmpl $1, %eax # check to see if systemcall is valid
    jl ret_neg_one
    cmpl $10, %eax
    jg ret_neg_one

    pushl %esi
    pushl %edi
    pushl %ebp
    pushl %edx
    pushl %ecx # push registers to stack
    pushl %ebx
    pushfl
    pushl %edx # push arguments 
    pushl %ecx
    pushl %ebx
	call *syscall_jmp_table(,%eax, 4) # call function from jmp table
    addl $12, %esp
    popfl
    popl %ebx
    popl %ecx
    popl %edx
    popl %ebp
    popl %edi
    popl %esi
    jmp done
ret_neg_one:
    movl $-1, %eax
done:
    iret # return from interrupt
syscall_jmp_table:
    .long temp
    .long halt
    .long execute
    .long read
    .long write
    .long open
    .long close
    .long getargs
    .long vidmap
    .long set_handler
    .long sigreturn

.end
