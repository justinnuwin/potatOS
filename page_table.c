#include "page_table.h"

#include <stdbool.h>
#include <stddef.h>
#include "multiboot2_tags.h"
#include "printk.h"
#include "string.h"
#include "interrupt.h"

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
    // uint8_t available0 : 3;             // 9 - 11 
    uint8_t allocated : 1;              // 9
    uint8_t copy_on_write : 1;          // 10
    uint8_t available0 : 1;             // 11
    uint8_t address_0_3 : 4;            // 12 - 15
    uint16_t address_4_19;              // 16 - 31
    uint16_t address_20_35;             // 32 - 47
    uint8_t address_36_39 : 4;          // 48 - 51
    uint8_t available1 : 4;             // 52 - 55
    uint8_t available2 : 7;             // 56 - 62
    uint8_t not_executable : 1;         // 63
} __attribute__ ((packed));

#define PAGETABLEADDRMASK 0xFFFFFFFFFF000   // Bits 12 - 51 used for address

union PageTable {
    struct PageTableEntry entry[512];
    void *entryAsAddr[512];
};

extern "C" union PageTable p4_table;
extern "C" union PageTable p3_table;
/* Page Map Level 4 Table Map
 *  Slot  |     Base Address    |      Usage   
 * -------|---------------------|------------------------
 *   0    | 0x0000000000        | Physical Page Frames
 * -------|---------------------|------------------------
 *   1    | 0x4000000000        | Kernel Stacks
 * -------|---------------------|------------------------
 *   2    | 0x8000000000        | Reserved for growth
 *  14    | 0x38000000000       | 
 * -------|---------------------|------------------------
 *  15    | 0x3c000000000       | Kernel Heap
 * -------|---------------------|------------------------
 *  16    | 0x40000000000       | User space
 *  32    |                     | 
 */
union PageTable *PTL4;  // Page Map Level 4 Table (PML4)
extern "C" void *kstack1 = (void *)(1 << 39);       // Layer 4 idx is 9 + 9 + 9 + 12 = 39
extern "C" void *kstack2 = (void *)(2 << 39);
extern "C" void *kstack3 = (void *)(3 << 39);
extern "C" void *kstack4 = (void *)(4 << 39);
extern "C" void *kstack5 = (void *)(5 << 39);
extern "C" void *kstack6 = (void *)(6 << 39);
extern "C" void *kstack7 = (void *)(7 << 39);
extern "C" void *kstack8 = (void *)(8 << 39);
extern "C" void *kstack9 = (void *)(9 << 39);
extern "C" void *kstack10 = (void *)(10 << 39);
extern "C" void *kstack11 = (void *)(11 << 39);
extern "C" void *kstack12 = (void *)(12 << 39);
extern "C" void *kstack13 = (void *)(13 << 39);
extern "C" void *kstack14 = (void *)(14 << 39);
const int heap_l4_idx = 15;
int heap_l3_idx = 0;
int heap_l2_idx = 0;
int heap_l1_idx = 0;

// Page Directory Pointer Table (PDP)
// Page Directory Table (PT)
// Page Table (PT)

static void *current_page;

void *MMU_pf_alloc();

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

inline void *get_address(union PageTable *pt, unsigned index) {
    return (void *)((unsigned long long)pt->entryAsAddr[index] & PAGETABLEADDRMASK);
}

void init_identity_map_table() {
    // boot.asm already allocated PTL4, PTL3, & PTL2 of PTL3[0] for us
    for (int i = 1; i < 512; i++) {
        union PageTable *ptl2 = (union PageTable *)MMU_pf_alloc();
        p3_table.entryAsAddr[i] = (void *)ptl2;
        p3_table.entry[i].present = 1;
        p3_table.entry[i].rw = 1;
        for (int j = 0; j < 512; j++) {
            ptl2->entryAsAddr[j] = (void *)(0x200000 * j + 0x40000000 * i);      // 2MiB := 0x20 0000     1GiB := 0x4000 0000
            ptl2->entry[j].present = 1;
            ptl2->entry[j].rw = 1;
            ptl2->entry[j].huge = 1;
        }
    }
}

extern "C" void page_fault_isr_wrapper(void);
void init_heap_pt() {
    heap_l3_idx = 0;
    heap_l2_idx = 0;
    heap_l1_idx = 0;
    union PageTable *l3 = (union PageTable *)MMU_pf_alloc();
    PTL4->entryAsAddr[15] = (void *)l3;
    PTL4->entry[15].present = 1;   PTL4->entry[15].rw = 1;
    union PageTable *l2 = (union PageTable *)MMU_pf_alloc();
    l3->entryAsAddr[0] = (void *)l2;
    l3->entry[0].present = 1;       l3->entry[0].rw = 1;
    union PageTable *l1 = (union PageTable *)MMU_pf_alloc();
    l2->entryAsAddr[0] = (void *)l1;
    l2->entry[0].present = 1;       l2->entry[0].rw = 1;
    register_isr(page_fault_isr_wrapper, 0xe);
}

void init_stacks_pt() {
   for (int i = 1; i < 15; i++) {
      PTL4.entry[i].present = 1;    PTL4.entry[i].rw = 1;
   }
}

void MMU_pf_init() {
    current_page = multiboot2_memory_map[0].start;
    PTL4 = &p4_table;
    init_identity_map_table();
    init_heap_pt();
    init_stacks_pt();
}

void *MMU_alloc_page() {
    // TODO: Traverse page tables to recoup free'ed frames
    union PageTable *ptl3 = (union PageTable *)get_address(PTL4, heap_l4_idx);
    union PageTable *ptl2 = (union PageTable *)get_address(ptl3, heap_l3_idx);
    union PageTable *ptl1 = (union PageTable *)get_address(ptl2, heap_l2_idx);
    ptl1->entry[heap_l1_idx].allocated = 1;
    void *v_addr = (void *)((heap_l4_idx << 39)      |       // Heap is index 15 << (9 + 9 + 9 + 12)
                            (heap_l3_idx << 30)      |
                            (heap_l2_idx << 21)      |
                            (heap_l1_idx << 12));
    heap_l1_idx++;
    if (heap_l1_idx >= 512) {
        heap_l2_idx++;
        if (heap_l2_idx >= 512) {
            heap_l3_idx++;
            if (heap_l3_idx >= 512) {
                printk("Kernel heap exceeded!\n");
                return NULL;
            } else {
                union PageTable *new_l2 = (union PageTable *)MMU_pf_alloc();
                ptl3->entryAsAddr[heap_l3_idx] = (void *)new_l2;
                ptl3->entry[heap_l3_idx].present = 1;   ptl3->entry[heap_l3_idx].rw = 1;
                heap_l2_idx = 0;
                union PageTable *new_l1 = (union PageTable *)MMU_pf_alloc();
                ptl2->entryAsAddr[heap_l2_idx] = (void *)new_l1;
                ptl2->entry[heap_l2_idx].present = 1;   ptl2->entry[heap_l2_idx].rw = 1;
                heap_l1_idx = 0;
            }
        } else {
            union PageTable *new_l1 = (union PageTable *)MMU_pf_alloc();
            ptl2->entryAsAddr[heap_l2_idx] = (void *)new_l1;
            ptl2->entry[heap_l2_idx].present = 1;   ptl2->entry[heap_l2_idx].rw = 1;
            heap_l1_idx = 0;
        }
    }
    return v_addr;
}

void *MMU_alloc_pages(int num) {
    void *v_addr = 0x0;
    do {
        num--;
        if (!v_addr)
            v_addr = MMU_alloc_page();
        else 
            MMU_alloc_page();
    } while (num);
    return v_addr;
}

void MMU_free_page(void *_vaddr) {
    uint64_t vaddr = (uint64_t)_vaddr;
    vaddr >>= 12;
    unsigned l1_idx = vaddr & 0x1ff;
    vaddr >>= 9;
    unsigned l2_idx = vaddr & 0x1ff;
    vaddr >>= 9;
    unsigned l3_idx = vaddr & 0x1ff;
    vaddr >>= 9;
    unsigned l4_idx = vaddr & 0x1ff;
    union PageTable *ptl3 = (union PageTable *)get_address(PTL4, l4_idx);
    union PageTable *ptl2 = (union PageTable *)get_address(ptl3, l3_idx);
    union PageTable *ptl1 = (union PageTable *)get_address(ptl2, l2_idx);
    void *phys_addr = (void *)(((uint64_t)ptl1->entryAsAddr[l1_idx]) & PAGETABLEADDRMASK);
    MMU_pf_free(phys_addr);
    ptl1->entryAsAddr[l1_idx] = 0;
}

void MMU_free_pages(void *v_addr, int num) {
    do {
        num--;
        MMU_free_page(v_addr);
        v_addr = (void *)((char *)v_addr + 4096);
    } while (num);
}

void page_fault_interrupt_handler(uint32_t code, uint64_t cr2) {
    // Error code description: https://wiki.osdev.org/Exceptions#Page_Fault
    cr2 >>= 12;
    unsigned l1_idx = cr2 & 0x1ff;
    cr2 >>= 9;
    unsigned l2_idx = cr2 & 0x1ff;
    cr2 >>= 9;
    unsigned l3_idx = cr2 & 0x1ff;
    cr2 >>= 9;
    unsigned l4_idx = cr2 & 0x1ff;
    union PageTable *ptl3 = (union PageTable *)get_address(PTL4, l4_idx);
    // Heap allocator should already mark L4 - L2 as present and rw
    // Stack will need to be set on the fly
    if (!(ptl3->entry[l3_idx].present) && l4_idx < 15) {
        ptl3->entry[l3_idx].present = 1;    ptl3->entry[l3_idx].rw = 1;
    }
    union PageTable *ptl2 = (union PageTable *)get_address(ptl3, l3_idx);
    if (!(ptl2->entry[l2_idx].present) && l4_idx < 15) {
        ptl2->entry[l2_idx].present = 1;    ptl2->entry[l2_idx].re = 1;
    }
    union PageTable *ptl1 = (union PageTable *)get_address(ptl2, l2_idx);
    ptl1->entryAsAddr[l1_idx] = MMU_pf_alloc();
    ptl1->entry[l1_idx].present = 1;    ptl1->entry[l1_idx].rw = 1;
    asm volatile ("invlpg %0" : : "m"(ptl1->entryAsAddr[l1_idx]));
}
