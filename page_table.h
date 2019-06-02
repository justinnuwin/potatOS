#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

void MMU_pf_init();
void *MMU_alloc_page();
void *MMU_alloc_pages(int num);
void MMU_free_page(void *);
void MMU_free_pages(void *, int num);

#endif
