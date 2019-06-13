#include "threading.h"

#include "kmalloc.h"

#define MAX_NUM_THREADS 14      // Set by number of PTL4 entries reserved for kernel stacks

struct KThread {
    int stack_num;
    kproc_t entry_point;
    void *args;
};

struct KThread *threads[MAX_NUM_THREADS + 1] = {0};    // Ignore index 0

void PROC_run();

typedef void (*kproc_t)(void *);
void PROC_create_kthread(kproc_t entry_point, void *args) {
    int stack_number;
    for (stack_number = 1; stack_number <= MAX_NUM_THREADS; stack_number++) {
        if (!threads[stack_number])
            break;
    }
    struct KThread *thread = (struct KThread *)kmalloc(sizeof(*thread));
    threads[stack_number] = thread;
    thread->stack_num = stack_number;
    thread->entry_point = entry_point;
    thread->args = args;
}

void PROC_reschedule();

void kexit();

