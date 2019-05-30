#ifdef __linux__
#error "Use a cross compiler!"
#endif

#ifndef KERNEL_H
#define KERNEL_H

extern "C" {
    void kernel_main(void *multiboot2_tag);
}

#endif
