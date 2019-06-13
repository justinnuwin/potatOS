#ifndef SCHEDULING_H
#define SCHEDULING_H

inline void yield(void) {
    asm volatile ("int 0x80");
}

#endif
