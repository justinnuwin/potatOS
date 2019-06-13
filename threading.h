#ifndef SCHEDULING_H
#define SCHEDULING_H

typedef void (*kproc_t)(void *);

void init_threading();
void PROC_run();
void PROC_create_kthread(kproc_t entry_point, void *args);

inline void yield(void) {
    asm volatile ("int $0x80");
}

#endif
