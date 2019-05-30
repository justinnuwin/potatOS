#include <stdint.h>

#ifndef MULTIBOOT2_TAGS_H
#define MULTIBOOT2_TAGS_H

struct MemoryMap {
    void *start;
    void *end;
};

extern struct MemoryMap *multiboot2_memory_map;
extern struct MemoryMap elf64_used_frames[];
extern unsigned short elf64_num_used_frames;

void read_multiboot2_tags(void *pointer);

#endif
