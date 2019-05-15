#include "gdt.h"

#include <stdint.h>
#include "string.h"

extern "C" const struct gdt_descriptor bs_gdt asm("gdt64.descriptor");

/*
 * Segment limit and base ignored in long mode (paging is enforced)
 */
struct code_segment_descriptor {
    uint16_t limit_0_15;            // 0 - 15
    uint16_t base_0_15;             // 16 - 32
    uint8_t  base_16_23;            // 32 - 39
    uint8_t  access : 1;            // 40
    uint8_t  readable : 1;          // 41
    uint8_t  conforming : 1;        // 42
    uint8_t  one1 : 1;              // 43
    uint8_t  one2 : 1;              // 44
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
    uint16_t limit_0_15;            // 0 - 15
    uint16_t base_0_15;             // 16 - 32
    uint8_t  base_16_23;            // 32 - 39
    uint8_t  type : 4;              // 
    uint8_t  zero0 : 1;
    uint8_t  protection_level : 2;
    uint8_t  present : 1;
    uint8_t  limit_16_19 : 4;
    uint8_t  available : 1;
    uint8_t  reserved0 : 2;
    uint8_t  g : 1;
    uint8_t  base_24_31;
    uint32_t base_32_63;
    uint8_t  reserved1;
    uint8_t  zero1 : 5;
    uint8_t  reserved2 : 3;
    uint32_t reserved3;
} __attribute__ ((packed));

struct tss {
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
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

void copy_bootstrap_gdt(struct gdt *gdt) {
    memcpy(gdt, bs_gdt.base, bs_gdt.size + 1);
}

void setup_tss() {
    static struct gdt gdt;
    copy_bootstrap_gdt(&gdt);
    lgdt(&gdt);
}
