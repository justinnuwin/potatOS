#ifndef KMALLOC_H
#define KMALLOC_H

void init_heap();
void *kmalloc(int size);
void kfree(void *addr);

#endif
