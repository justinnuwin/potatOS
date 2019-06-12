#include <stddef.h>
#include "page_table.h"

struct KmallocPool {
    struct KmallocHeader *head;
};

struct KmallocHeader {
    union {                         // 8 bytes
        struct KmallocPool *pool;       // Parent pool if used
        struct KmallocHeader *next;     // Next free header if free
    } u;
    int32_t size;                   // 4 bytes  (size of data when used, size of block when free)
    int32_t available;              // 4 bytes  (used = 0; free = 1)
} __attribute__ ((packed));

/* Kmalloc Structure
struct Kmalloc {
    struct KmallocHeader;
    void data;      // Start is 16 byte aligned
    char padding;
};
*/

struct KmallocPool b32_pool = {0};
struct KmallocPool b64_pool = {0};
struct KmallocPool b128_pool = {0};
struct KmallocPool b256_pool = {0};
struct KmallocPool b512_pool = {0};
struct KmallocPool b1024_pool = {0};
struct KmallocPool b2048_pool = {0};

void *brk;
static void *current_page;

void init_heap() {
    brk = MMU_alloc_page();
    current_page = brk;
}

int get_min_block_size(int size) {
    // TODO: Try floating pt div <= 1
    if (size / 32 <= 0 || size == 32)
        return 32;
    else if (size / 64 <= 0 || size == 64)
        return 64;
    else if (size / 128 <= 0 || size == 128)
        return 128;
    else if (size / 256 <= 0 || size == 256)
        return 256;
    else if (size / 512 <= 0 || size == 512)
        return 512;
    else if (size / 1024 <= 0 || size == 1024)
        return 1024;
    else if (size / 2048 <= 0 || size == 2048)
         return 2048;
    else
        return NULL;
}

void append_pool(void *addr, int block_size) {
    struct KmallocPool *pool;
    switch (block_size) {
        case 32:
            pool = &b32_pool;
            break;
        case 64:
            pool = &b64_pool;
            break;
        case 128:
            pool = &b128_pool;
            break;
        case 256:
            pool = &b256_pool;
            break;
        case 512:
            pool = &b512_pool;
            break;
        case 1024:
            pool = &b1024_pool;
            break;
        case 2048:
            pool = &b2048_pool;
            break;
    }
    if (!(pool->head)) {
        pool->head = (struct KmallocHeader *)addr;
        pool->head->u.next = NULL;
        pool->head->size = block_size;
        pool->head->available = 1;
    } else {
        struct KmallocHeader *header = pool->head;
        while (header->u.next)
            header = header->u.next;
        header->u.next = (struct KmallocHeader *)addr;
        header->u.next->size = block_size;
        header->u.next->available = 1;
    }
}

void *kmalloc(int size) {
    if (size <= 0)
        return NULL;

    int full_size = size + sizeof(struct KmallocHeader);
    int block_size = get_min_block_size(full_size);
    
    if ((uint64_t)brk + full_size > (uint64_t)current_page + 4096) {
        int page_remaining = (uint64_t)current_page + 4096 - (uint64_t)brk;
        while (page_remaining > 0) {
            int _block_size = get_min_block_size(page_remaining);
            while (_block_size > page_remaining)
                _block_size >>= 1;
            append_pool(brk, _block_size);
            brk = (char *)brk + _block_size;
            page_remaining -= _block_size;
            brk = MMU_alloc_page();
            current_page = brk;
        }
    }
    ((struct KmallocHeader *)brk)->u.next = NULL;
    ((struct KmallocHeader *)brk)->size = size;
    brk = (char *)brk + block_size;
    return (char *)brk + sizeof(struct KmallocHeader);
}

void kfree(void *addr);
