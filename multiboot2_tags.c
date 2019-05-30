#include "multiboot2_tags.h"

#include <stddef.h>
#include "printk.h"

#define MEM_MAP_FREE_REGION_TYPE 1

struct Multiboot2Header {
    uint32_t size;
    uint32_t reserved;
} __attribute__ ((packed));

struct Multiboot2TagHeader {
    uint32_t type;
    uint32_t size;
    union {
        char boot_cmd_line;         // Tag type 1
        char bootloader_name;       // Tag type 2
        struct {                    // Tag type 4
            uint32_t low_mem_size;
            uint32_t high_mem_size;
        } mem_info __attribute__ ((packed));
        struct {                    // Tag type 5
            uint32_t boot_device;
            uint32_t boot_partition;
            uint32_t boot_subpartition;
        } boot_device __attribute__ ((packed));
        struct {                    // Tag type 6
            uint32_t mem_entry_size;
            uint32_t mem_entry_version;
            struct {
                uint32_t start;
                uint32_t length;
                uint32_t type;
                uint32_t reserved;
            } memory_entry_start __attribute__ ((packed));
        } mem_map __attribute__ ((packed));
    } data;
} __attribute__ ((packed));

struct Multiboot2TagHeader *next_tag(struct Multiboot2TagHeader *tag, uint32_t &total_size) {
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
    return (struct Multiboot2TagHeader *)next_tag;
}

void parse_memory_map_tag(struct Multiboot2TagHeader *tag) {
}

void parse_tag(struct Multiboot2TagHeader *tag) {
    switch (tag->type) {
        case 0:     // NULL (end of tags)
            break;
        case 1:     // Boot Command Line
            printk("Boot command line: %s\n", &(tag->data.boot_cmd_line));
            break;
        case 2:     // Bootloader Name
            printk("Bootloader name: %s\n", &(tag->data.bootloader_name));
            break;
        case 4:     // Memory Info
            printk("Low  memory size (bytes): %u\n", tag->data.mem_info.low_mem_size);
            printk("High memory size (bytes): %u\n", tag->data.mem_info.high_mem_size);
            break;
        case 5:     // BIOS Boot Device
            printk("Boot device: %x\tBoot Partition: %x Subpartition %x\n", 
                    tag->data.boot_device.boot_device,
                    tag->data.boot_device.boot_partition,
                    tag->data.boot_device.boot_subpartition
                  );
            break;
        case 6:     // Memory Map
            parse_memory_map_tag(tag);
            break;
        case 9:     // ELF Symbols
            break;
        default:
            printk("Unsupported multiboot2 tag: %u\n", tag->type);
            break;
    }
}

void read_multiboot2_tags(void *pointer) {
    struct Multiboot2Header *header = (struct Multiboot2Header *)pointer;
    uint32_t size_read = header->size - sizeof(*header);
    struct Multiboot2TagHeader *tag = (struct Multiboot2TagHeader *)((uint8_t *)pointer + sizeof(*header));
    parse_tag(tag);
    while ((tag = next_tag(tag, size_read))) {
        parse_tag(tag);
    }
}

