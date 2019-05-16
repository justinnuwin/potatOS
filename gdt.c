#include "gdt.h"

#include <stdint.h>
#include "string.h"

extern "C" const struct gdt_descriptor bs_gdt asm("gdt64.descriptor");
extern "C" void *stack_top1;
extern "C" void *stack_top2;
extern "C" void *stack_top3;
extern "C" void *stack_top4;
extern "C" void *stack_top5;
extern "C" void *stack_top6;
extern "C" void *stack_top7;

/*
 * Segment limit and base ignored in long mode (paging is enforced)
 */
struct code_segment_descriptor {
    uint16_t limit_0_15;            // 0 - 15   (8 byte GDT entry)
    uint16_t base_0_15;             // 16 - 32
    uint8_t  base_16_23;            // 32 - 39
    uint8_t  access : 1;            // 40
    uint8_t  readable : 1;          // 41
    uint8_t  conforming : 1;        // 42
    uint8_t  executable : 1;        // 43
    uint8_t  type : 1;              // 44
    uint8_t  protection_level : 2;  // 45 - 46
    uint8_t  present : 1;           // 47
    uint8_t  limit_16_19 : 4;       // 48 - 51
    uint8_t  available : 1;         // 52
    uint8_t  long_mode : 1;         // 53
    uint8_t  default_op_size : 1;   // 54
    uint8_t  granularity : 1;       // 55
    uint8_t  base_24_31;            // 56 - 63
} __attribute__ ((packed));

struct task_state_segment_descriptor {
    uint16_t limit_0_15;            // 0 - 15   (8 byte GDT entry)
    uint16_t base_0_15;             // 16 - 32
    uint8_t  base_16_23;            // 32 - 39
    uint8_t  access : 1;            // 40
    uint8_t  readable : 1;          // 41
    uint8_t  conforming : 1;        // 42
    uint8_t  executable : 1;        // 43
    uint8_t  type : 1;              // 44
    uint8_t  protection_level : 2;  // 45 - 46
    uint8_t  present : 1;           // 47
    uint8_t  limit_16_19 : 4;       // 48 - 51
    uint8_t  available : 1;         // 52
    uint8_t  long_mode : 1;         // 53
    uint8_t  size : 1;              // 54
    uint8_t  granularity : 1;       // 55
    uint8_t  base_24_31;            // 56 - 63
    uint32_t base_32_63;            // 64 - 95  (10 byte extension)
    uint8_t  reserved1;             // 96 - 103
    uint8_t  zero1 : 5;             // 104 - 108
    uint8_t  reserved2 : 3;         // 109 - 111
    uint32_t reserved3;             // 112 - 133
} __attribute__ ((packed));

struct tss {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    void **ist1;
    void **ist2;
    void **ist3;
    void **ist4;
    void **ist5;
    void **ist6;
    void **ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t io_map_base_addr;
} __attribute__ ((packed));

struct gdt_descriptor {
    uint16_t size;
    void *base;
} __attribute__ ((packed));

struct gdt {
    uint64_t zero;
    struct code_segment_descriptor csd;
    struct task_state_segment_descriptor tss;
} __attribute__ ((packed));

static struct gdt_descriptor GDTR;

void lgdt(struct gdt *gdt) {
    GDTR = {sizeof(*gdt) - 1, gdt};

    asm volatile ("lgdt %0" : : "m"(GDTR));
}

void ltr(struct gdt *gdt) {
    uint16_t tss_offset = (uint16_t)((uint64_t)&(gdt->tss) - (uint64_t)gdt);

    asm volatile ("ltr %0" : : "m"(tss_offset));
}

void copy_bootstrap_gdt(struct gdt *gdt) {
    memcpy(gdt, bs_gdt.base, bs_gdt.size + 1);
}

struct tss *setup_tss() {
    static struct tss tss;
    memset(&tss, 0, sizeof(tss));
    tss.ist1 = &stack_top1;
    tss.ist2 = &stack_top2;
    tss.ist3 = &stack_top3;
    tss.ist4 = &stack_top4;
    tss.ist5 = &stack_top5;
    tss.ist6 = &stack_top6;
    tss.ist7 = &stack_top7;
    tss.io_map_base_addr = sizeof(tss);
    return &tss;
}

void setup_gdt_tss() {
    static struct gdt gdt;
    copy_bootstrap_gdt(&gdt);
    struct tss *tss = setup_tss();
    uint16_t tss_limit = sizeof(*tss);
    gdt.tss.limit_0_15 = tss_limit & 0xffffffff;
    gdt.tss.limit_16_19 = (uint8_t)((tss_limit >> 16) & 0xffff);
    gdt.tss.base_0_15 =   (uint16_t) ((uint64_t)tss & 0xffffffff);
    gdt.tss.base_16_23 =  (uint8_t)  ((((uint64_t)tss) >> 16) & 0xffffffff);
    gdt.tss.base_24_31 =  (uint8_t)  ((((uint64_t)tss) >> 24) & 0xffffffff);
    gdt.tss.base_32_63 =  (uint32_t) ((((uint64_t)tss) >> 32) & 0xffffffff);
    gdt.tss.access = 1;
    gdt.tss.executable = 1;
    gdt.tss.present = 1;
    gdt.tss.size = 1;
    lgdt(&gdt);
    ltr(&gdt);
}
