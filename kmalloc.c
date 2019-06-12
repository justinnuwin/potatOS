#include <stddef.h>
#include "page_table.h"

struct KmallocPool {
    struct KmallocHeader *head;
    int block_size;
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

struct KmallocPool b32_pool = {NULL, 32};
struct KmallocPool b64_pool = {NULL, 64};
struct KmallocPool b128_pool = {NULL, 128};
struct KmallocPool b256_pool = {NULL, 256};
struct KmallocPool b512_pool = {NULL, 512};
struct KmallocPool b1024_pool = {NULL, 1024};
struct KmallocPool b2048_pool = {NULL, 2048};

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

struct KmallocPool *get_pool(int block_size) {
    switch (block_size) {
        case 32:
            return &b32_pool;
        case 64:
            return &b64_pool;
        case 128:
            return &b128_pool;
        case 256:
            return &b256_pool;
        case 512:
            return &b512_pool;
        case 1024:
            return &b1024_pool;
        case 2048:
            return &b2048_pool;
    }
}

void append_pool(struct KmallocHeader *header) {
    struct KmallocPool *pool = header->u.pool;
    if (!(pool->head)) {
        pool->head = header;
        pool->head->u.next = NULL;
        pool->head->size = pool->block_size;
        pool->head->available = 1;
    } else {
        struct KmallocHeader *free_header = pool->head;
        while (free_header->u.next)
            free_header = free_header->u.next;
        free_header->u.next = header;
        header->u.next->size = pool->block_size;
        header->u.next->available = 1;
    }
}

// addr should be struct KmallocHeader *
void append_pool(void *addr, int block_size) {
    struct KmallocPool *pool = get_pool(block_size);
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

    struct KmallocPool *pool = get_pool(block_size);
    if (pool->head) {
        struct KmallocHeader *allocated = pool->head;
        pool->head = pool->head->u.next;
        allocated->u.pool = pool;
        allocated->size = size;
        allocated->available = 0;
        return (char *)allocated + sizeof(struct KmallocHeader);
    }
    
    // Set rest of page as free and allocate this obj on next page frame
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
    ((struct KmallocHeader *)brk)->u.pool = pool;
    ((struct KmallocHeader *)brk)->size = size;
    void *allocated = (char *)brk + sizeof(struct KmallocHeader);
    brk = (char *)brk + block_size;
    return allocated;
}

void kfree(void *addr) {
    struct KmallocHeader *header = (struct KmallocHeader *)((char *)addr - sizeof(struct KmallocHeader));
    append_pool(header);
}
