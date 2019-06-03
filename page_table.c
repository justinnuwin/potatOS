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

// Bits 0 - 51 are the phyical address of the frame/next table with
// the lower bits being OR-ed in
struct PageTableEntry {
    uint8_t present : 1;                // 0
    uint8_t rw : 1;                     // 1
    uint8_t user : 1;                   // 2
    uint8_t write_thru_cache : 1;       // 3
    uint8_t disable_cache : 1;          // 4
    uint8_t accessed : 1;               // 5
    uint8_t dirty : 1;                  // 6
    uint8_t huge : 1;                   // 7
    uint8_t global : 1;                 // 8
    uint8_t available0 : 3;             // 9 - 11 
    uint8_t address_0_3 : 4;            // 12 - 15
    uint16_t address_4_19;              // 16 - 31
    uint16_t address_20_35;             // 32 - 47
    uint8_t address_36_39 : 4;          // 48 - 51
    uint8_t available1 : 4;             // 52 - 55
    uint8_t available2 : 7;             // 56 - 62
    uint8_t not_executable : 1;         // 63
} __attribute__ ((packed));

struct PageTable {
    struct PageTableEntry entry[512];
} __attribute__ ((packed, aligned(4096)));

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
struct PageTable *PTL4;  // Page Map Level 4 Table (PML4)
unsigned heap_l3_idx = 0;
unsigned heap_l2_idx = 0;
unsigned heap_l1_idx = 0;

// Page Directory Pointer Table (PDP)
// Page Directory Table (PT)
// Page Table (PT)

void *current_page;

void *MMU_pf_alloc();

extern "C" struct PageTableEntry p4_table;
void MMU_pf_init() {
    current_page = multiboot2_memory_map[0].start;
    PTL4 = (struct PageTable *)(&p4_table);
    PTL4->entry[1].present = 1;    PTL4->entry[1].rw = 1;
    PTL4->entry[2].present = 1;    PTL4->entry[2].rw = 1;
    PTL4->entry[3].present = 1;    PTL4->entry[3].rw = 1;
    PTL4->entry[4].present = 1;    PTL4->entry[4].rw = 1;
    PTL4->entry[5].present = 1;    PTL4->entry[5].rw = 1;
    PTL4->entry[6].present = 1;    PTL4->entry[6].rw = 1;
    PTL4->entry[7].present = 1;    PTL4->entry[7].rw = 1;
    PTL4->entry[8].present = 1;    PTL4->entry[8].rw = 1;
    PTL4->entry[9].present = 1;    PTL4->entry[9].rw = 1;
    PTL4->entry[10].present = 1;   PTL4->entry[10].rw = 1;
    PTL4->entry[11].present = 1;   PTL4->entry[11].rw = 1;
    PTL4->entry[12].present = 1;   PTL4->entry[12].rw = 1;
    PTL4->entry[13].present = 1;   PTL4->entry[13].rw = 1;
    PTL4->entry[14].present = 1;   PTL4->entry[14].rw = 1;
    PTL4->entry[15].present = 1;   PTL4->entry[15].rw = 1;
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

inline void *MMU_get_next_pf() {
    if (!address_used(current_page)) {
        void *alloced_page = current_page;
        current_page = (char *)current_page + PAGE_SIZE;
        return alloced_page;
    } else {
        current_page = (char *)current_page + PAGE_SIZE;
        return MMU_pf_alloc();
    }
}

inline void MMU_increment_available_memory_map_region() {
    multiboot2_memory_map = (struct MemoryMap *)multiboot2_memory_map + 1;
    current_page = multiboot2_memory_map->start;
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
            if ((char *)multiboot2_memory_map->end - (char *)current_page < PAGE_SIZE)
                MMU_increment_available_memory_map_region();
            return MMU_get_next_pf();
    } else {
        MMU_increment_available_memory_map_region();
        return MMU_get_next_pf();
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

void *get_address(struct PageTable *pt, unsigned index) {
    struct PageTableEntry entry = pt->entry[index];
    void *address = (void *)((entry.address_0_3 << 12)      |
                             (entry.address_4_19 << 16)     |
                             (entry.address_20_35 << 32)    |
                             (entry.address_36_39 << 48));
    return address;
    // struct PageTable *ptl3 = (struct PageTable *)((void *)(PTL4->entry[15]) & (0xFFFFFFFFFF000));     // Mask everything but address
}

void *MMU_alloc_page() {
    // TODO: Traverse page tables to recoup free'ed frames
    struct PageTable *ptl3 = (struct PageTable *)get_address(PTL4, 15);
    struct PageTable *ptl2 = (struct PageTable *)get_address(ptl3, heap_l3_idx);
    struct PageTable *ptl1 = (struct PageTable *)get_address(ptl2, heap_l2_idx);
    ptl1->entry[heap_l1_idx].present = 1;
    ptl1->entry[heap_l1_idx].rw = 1;
    unsigned long long offset = (unsigned long long)get_address(ptl1, heap_l1_idx);
    heap_l1_idx++;
    if (heap_l1_idx >= 512) {
        heap_l1_idx = 0;
        heap_l2_idx++;
        ptl2->entry[heap_l2_idx].present = 1;
        ptl2->entry[heap_l2_idx].rw = 1;
        if (heap_l2_idx >= 512) {
            heap_l2_idx = 0;
            heap_l3_idx++;
            ptl3->entry[heap_l3_idx].present = 1;
            ptl3->entry[heap_l3_idx].rw = 1;
            if (heap_l3_idx >= 512) {
                printk("Kernel heap exceeded!\n");
                return NULL;
            }
        }
    }
    void *phys_addr = (void *)((15 << 39)                   |       // Heap is index 15 << (9 + 9 + 9 + 12)
                               ((heap_l3_idx & 9) << 30)    |
                               ((heap_l2_idx & 9) << 21)    |
                               ((heap_l1_idx & 9) << 12))   +
                              offset;
    return phys_addr;
}

void *MMU_alloc_pages(int num) {}
void MMU_free_page(void *) {}
void MMU_free_pages(void *, int num) {}
