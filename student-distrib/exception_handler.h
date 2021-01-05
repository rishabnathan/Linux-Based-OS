#ifndef EXCEPTION_HANDLER_H
#define EXCEPTION_HANDLER_H

extern void divide_handler();
extern void debug_handler();
extern void NMI_handler();
extern void breakpoint_handler();
extern void overflow_handler();
extern void bound_range_handler();
extern void inv_opcode_handler();
extern void dev_not_available_handler();
extern void double_fault_handler();
extern void coprocessor_segment_handler();
extern void invalid_TSS_handler();
extern void segment_notpresent_handler();
extern void stack_fault_handler();
extern void general_protection_handler();
extern void page_fault_handler();
extern void fpu_floating_point_handler();
extern void alignment_handler();
extern void machine_check_handler();
extern void simd_floating_point_handler();
extern void system_call_handler();
#endif
