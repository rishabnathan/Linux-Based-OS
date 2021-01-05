#ifndef _PAGING_H
#define _PAGING_H

#define DIR_SIZE 1024
#define TABLE_SIZE 1024

#define KERNEL_ADDRESS 0x00400000
#define _4_KB 0x1000
#define _4_MB 0x400000
#define _8_MB 0x800000
#define _8_KB 0x2000
#define _128_MB 0x8000000
#define _132_MB 0x8400000
#define USER_PAGE 0x8048000 >> 22
#define USER_PAGE_OFFSET 0x8048000

/* Not present and read/write set */
#define ABSENT_ENTRY 0x10

/* Shift 22 bits for the PDE idx */
#define GET_PDE_IDX(a)  ((a) >> 22)
/* Shift 12 bits and mask for the PTE idx */
#define GET_PTE_IDX(a)  (((a) >> 12) & 0x3FF)

#include "lib.h"
#include "types.h"
uint32_t page_dir[DIR_SIZE] __attribute__((aligned (DIR_SIZE * sizeof(uint32_t))));
uint32_t page_table_init[TABLE_SIZE] __attribute__((aligned (TABLE_SIZE * sizeof(uint32_t))));
uint32_t vidmap_table[TABLE_SIZE] __attribute__((aligned (TABLE_SIZE * sizeof(uint32_t))));

void init_paging(void);
void map_page(uint32_t v_addr, uint32_t p_addr);
void flush_tlb();

#endif /* _PAGING_H */
