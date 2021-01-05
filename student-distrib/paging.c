#include "paging.h"



/*
 * init_paging
 * DESCRIPTION: initialize paging and set directory/jump_table
 * INPUTS: none
 * RETURN VALUE: none
 */
void init_paging(void) {
    /* PDE for init pages. Present bit and read/write. */
    uint32_t pte_init = ((uint32_t) page_table_init) | 0x3;
    /* Set all pages in page table as absent, but set read/write */
    memset(page_table_init, ABSENT_ENTRY, TABLE_SIZE);
    /* Present bit, read/write. */
    page_table_init[GET_PTE_IDX(VIDEO)] = VIDEO | 0x3;
    page_table_init[GET_PTE_IDX(VIDEO_TERM1)] = VIDEO_TERM1 | 0x3;
    page_table_init[GET_PTE_IDX(VIDEO_TERM2)] = VIDEO_TERM2 | 0x3;
    page_table_init[GET_PTE_IDX(VIDEO_TERM3)] = VIDEO_TERM3 | 0x3;
    /* Present bit, read/write, and 4 MB */
    uint32_t kernel_page = KERNEL_ADDRESS | 0x83;
    memset(page_dir, ABSENT_ENTRY, DIR_SIZE);
    page_dir[0] = pte_init;
    page_dir[1] = kernel_page;

    /* Enable paging in cr0 and address of directory in cr3. Also enables PSE */
    asm volatile ("             \n\
      movl  %0, %%eax           \n\
      movl  %%eax, %%cr3        \n\
      movl  %%cr4, %%eax        \n\
      orl   $0x00000010, %%eax  \n\
      movl  %%eax, %%cr4        \n\
      movl  %%cr0, %%eax        \n\
      orl   $0x80000000, %%eax  \n\
      movl  %%eax, %%cr0        \n\
      "
      : /* no outputs */
      : "g" (page_dir)
      : "eax" /* Clobbers eax */
    );
}

void map_page(uint32_t v_addr, uint32_t p_addr) {
  uint32_t pd_idx = v_addr / _4_MB;
  /* Set user, present, r/w, and 4 MB */
  page_dir[pd_idx] = p_addr | 0x87;
  flush_tlb();
}

void flush_tlb() {
  asm volatile("                \n\
    movl %%cr3, %%eax           \n\
    movl %%eax, %%cr3           \n\
    "
    : /* no outputs */
    : /* no inputs */
    : "eax" /* Clobbers eax */
  );
}
