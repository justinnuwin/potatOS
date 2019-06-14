#include "syscall.h"

#include <stdint.h>

extern void context_switch(void *rsp);      // threading.c

extern "C" void sys_call_interrupt_handler(int number, void *rsp) {
    switch (number) {
        case SYSCALL_SCHED_YIELD:
            context_switch(rsp);
            break;
    }
}
