#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

#include <stdint.h>

void MMU_pf_init();
void *MMU_alloc_page();
void *MMU_alloc_pages(int num);
void MMU_free_page(void *);
void MMU_free_pages(void *, int num);
void free_stack(int stack_number);
extern "C" void page_fault_interrupt_handler(uint32_t code, uint64_t cr2);

#endif
