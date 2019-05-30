#ifndef PAGE_TABLE_H
#define PAGE_TABLE_H

void MMU_pf_init();
void *MMU_pf_alloc();
void MMU_pf_free(void *);

#endif
