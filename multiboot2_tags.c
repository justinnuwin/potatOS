#include "multiboot2_tags.h"

#include <stdint.h>
#include <stddef.h>
#include "printk.h"

struct multiboot2_header {
    uint32_t size;
    uint32_t reserved;
} __attribute__ ((packed));

struct multiboot2_tag_header {
    uint32_t type;
    uint32_t size;
    void *data;      // Variable size data, not a pointer
} __attribute__ ((packed));

struct multiboot2_tag_header *next_tag(struct multiboot2_tag_header *tag, uint32_t &total_size) {
    total_size -= tag->size;
    if (total_size == 0)
        return NULL;
    if (tag->type == 0 && tag->size == 8)
        return NULL;


    void *next_tag = NULL;
    if (tag->size % 8 == 0)
        next_tag = (uint8_t *)tag + tag->size;
    else
        next_tag = (uint8_t *)tag + tag->size + (8 - (tag->size % 8));
    return (struct multiboot2_tag_header *)next_tag;
}

void parse_tag(struct multiboot2_tag_header *tag) {
    switch (tag->type) {
        case 0:     // NULL (end of tags)
            break;
        case 1:     // Boot Command Line
            printk("Boot command line: %s\n", (char *)&(tag->data));
            break;
        case 2:     // Bootloader Name
            printk("Bootloader name: %s\n", (char *)&(tag->data));
            break;
        case 4:     // Memory Info
            break;
        case 5:     // BIOS Boot Device
            break;
        case 6:     // Memory Map
            break;
        case 9:     // ELF Symbols
            break;
        default:
            printk("Unsupported multiboot2 tag: %u\n", tag->type);
            break;
    }
}

void read_multiboot2_tags(void *pointer) {
    struct multiboot2_header *header = (struct multiboot2_header *)pointer;
    uint32_t size_read = header->size - sizeof(*header);
    struct multiboot2_tag_header *tag = (struct multiboot2_tag_header *)((uint8_t *)pointer + sizeof(*header));
    parse_tag(tag);
    while ((tag = next_tag(tag, size_read))) {
        parse_tag(tag);
    }
}


