#include "page_table.h"

#include <stdbool.h>
#include <stddef.h>
#include "multiboot2_tags.h"
#include "printk.h"

#define PAGE_SIZE 4096

struct FreePageLinkList {
    struct FreePageLinkList *next;
};

struct FreePageLinkList *free_pages_head = NULL;

struct CR3 {
    uint32_t reserved0 : 3;
    uint32_t pwt : 1;
    uint32_t pcd : 1;
    uint32_t reserved1 : 7;
    uint32_t base_0_19 : 20;
    uint32_t base_20_39 : 20;
    uint32_t reserved2 : 12;
};

struct PTL4Entry {  // Entry in the Page Map :evel 4 Table (PML4)
} __attribute__ ((packed));

struct PTL3Entry {  // Entry in the Page Directory Pointer Table (PDP)
} __attribute__ ((packed));

struct PTL2Entry {  // Entry in the Page Directory Table (PT)
} __attribute__ ((packed));

struct PTL1Entry {  // Entry in the Page Table (PT)
} __attribute__ ((packed));

/* Page Map Level 4 Table Map
 *  Slot  |     Base Address    |      Usage   
 * -------|---------------------|------------------------
 *   0    | 0x0000000000        | Physical Page Frames
 * -------|---------------------|------------------------
 *   1    | 0x1000000000        | Kernel Stacks
 * -------|---------------------|------------------------
 *   2    | 0x2000000000        | Reserved for growth
 *  14    | 0xEFFFFFFFFFF       | 
 * -------|---------------------|------------------------
 *  15    | 0xF0000000000       | Kernel Heap
 * -------|---------------------|------------------------
 *  16    | 0x100000000000      | User space
 *  32    |                     | 
 */
struct PTL4Entry PTL4[32];

void *current_page;

void MMU_pf_init() {
    current_page = multiboot2_memory_map[0].start;
}

bool address_used(void *address) {
    int idx = 0;
    while (idx < elf64_num_used_frames) {
        if ((elf64_used_frames[idx].start >= address && elf64_used_frames[idx].start <= (char *)address + PAGE_SIZE) ||
            (elf64_used_frames[idx].end >= address && elf64_used_frames[idx].end <= (char *)address + PAGE_SIZE) ||
            (address >= elf64_used_frames[idx].start && address <= elf64_used_frames[idx].end))
            return true;
        idx++;
    }
    return false;
}

void *MMU_pf_alloc() {
    if (free_pages_head) {
        void *alloced_page = free_pages_head;
        free_pages_head = free_pages_head->next;
        return alloced_page;
    }

    if (multiboot2_memory_map->start == 0 &&
        multiboot2_memory_map->end   == 0) {
        printk("End of available memory regions!\n");
        return NULL;
    }
    if (current_page >= multiboot2_memory_map->start &&
        current_page <= multiboot2_memory_map->end) {
            if ((unsigned long long)multiboot2_memory_map->end - (unsigned long long)current_page < PAGE_SIZE) {    // Next page is smaller than PAGE_SIZE
                multiboot2_memory_map = (struct MemoryMap *)multiboot2_memory_map + 1;
                current_page = multiboot2_memory_map->start;
            }

            if (!address_used(current_page)) {
                void *alloced_page = current_page;
                current_page = (char *)current_page + PAGE_SIZE;
                return alloced_page;
            } else {
                current_page = (char *)current_page + PAGE_SIZE;
                return MMU_pf_alloc();
            }
    } else {
        multiboot2_memory_map = (struct MemoryMap *)multiboot2_memory_map + 1;
        current_page = multiboot2_memory_map->start;
        if (!address_used(current_page)) {
            void *alloced_page = current_page;
            current_page = (char *)current_page + PAGE_SIZE;
            return alloced_page;
        } else {
            current_page = (char *)current_page + PAGE_SIZE;
            return MMU_pf_alloc();
        }
    }
}

void MMU_pf_free(void *address) {
    if (!free_pages_head) {
        free_pages_head = (struct FreePageLinkList *)address;
        free_pages_head->next = NULL;
    } else {
        struct FreePageLinkList *node = free_pages_head;
        while (node->next)
            node = node->next;
        node->next = (struct FreePageLinkList *)address;
    }
}
