#ifndef _IDT_H
#define _IDT_H

#include "keyboard.h"
#define INTEL_RESERVED_VECS 32
#define KEYBOARD_VEC 33
#define RTC_VEC 40
#define PIT_VEC 32
#define SYSTEM_CALL_VEC 0x80



void init_idt();
#endif
