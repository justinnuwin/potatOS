#include "multiboot2_tags.h"

#include <stddef.h>
#include "printk.h"

#define MEM_MAP_FREE_REGION_TYPE 1

#define ELF64_SHF_WRITE     0x1

struct MemoryMap _multiboot2_memory_map[256];
struct MemoryMap *multiboot2_memory_map = _multiboot2_memory_map;
struct MemoryMap elf64_used_frames[256];
unsigned short elf64_num_used_frames;

struct Multiboot2Header {
    uint32_t size;
    uint32_t reserved;
} __attribute__ ((packed));

struct MemoryMapEntry {
    void *start;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} __attribute__ ((packed));

struct ELF64SectionHeader {
    uint32_t name_idx;
    uint32_t type;
    uint64_t flags;
    void *segment_address;
    uint64_t segment_offset;
    uint64_t segment_size;      // bytes
    uint32_t link_idx;
    uint32_t extra_info;
    uint64_t address_alignment;
    uint64_t fixed_entry_size;
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
            struct MemoryMapEntry memory_entry_start;
        } mem_map __attribute__ ((packed));
        struct {                    // Tag type 9
            uint32_t num_headers;
            uint32_t size_headers;
            uint32_t string_table_idx;
            struct ELF64SectionHeader section_header_start;
        } elf_sym __attribute__ ((packed));
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

// Returns number of free memory regions found
int parse_memory_map_tag(struct Multiboot2TagHeader *tag) {
    uint8_t mem_map_idx = 0;
    struct MemoryMapEntry *mem_map_entry = (struct MemoryMapEntry *)&(tag->data.mem_map.memory_entry_start);
    uint32_t size_read = tag->size - sizeof(tag->type) - sizeof(tag->size) - sizeof(tag->data.mem_map.mem_entry_size) - sizeof(tag->data.mem_map.mem_entry_version);
    while (size_read > 0) {
        if (mem_map_entry->type == MEM_MAP_FREE_REGION_TYPE) {
            multiboot2_memory_map[mem_map_idx].start = mem_map_entry->start;
            multiboot2_memory_map[mem_map_idx].end = (uint8_t *)mem_map_entry->start + mem_map_entry->length;
            mem_map_idx++;
        }
        mem_map_entry++;
        size_read -= sizeof(*mem_map_entry);
    }
    return (int)mem_map_idx;
}

// Returns number of free memory regions found
int parse_elf_symbols(struct Multiboot2TagHeader *tag) {
    uint8_t mem_map_idx = 0;
    struct ELF64SectionHeader *header = (struct ELF64SectionHeader *)&(tag->data.elf_sym.section_header_start);
    uint32_t header_counter = tag->data.elf_sym.num_headers;
    while (header_counter-- > 0) {
        elf64_used_frames[mem_map_idx].start = header->segment_address;
        elf64_used_frames[mem_map_idx].end = (uint8_t *)header->segment_address + header->segment_size;
        mem_map_idx++;
        header++;
    }
    elf64_num_used_frames = mem_map_idx;
    return elf64_num_used_frames;
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
            printk("Identified %d free memory regions!\n", parse_memory_map_tag(tag));
            break;
        case 9:     // ELF Symbols
            printk("Identified %d used memory regions!\n", parse_elf_symbols(tag)); 
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

