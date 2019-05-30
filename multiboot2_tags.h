#include <stdint.h>

#ifndef MULTIBOOT2_TAGS_H
#define MULTIBOOT2_TAGS_H

struct MemoryMap {
    void *start;
    uint32_t length;
};

struct MemoryMap memory_map[256];

void read_multiboot2_tags(void *pointer);

#endif
