#include "kmalloc.h"

extern "C" void *ckmalloc(int size) {
    return kmalloc(size);
}

extern "C" void ckfree(void *addr) {
    return kfree(addr);
}
