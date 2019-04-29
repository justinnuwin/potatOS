global isr_wrapper
extern interrupt_handler
align  4

section .text
bits 64
isr_wrapper:
    ; pushad    ; pushad is not a valid op in 64-bit mode
    ; push 64-bit callee saved registers. Assume C compiler will preserve:
    ; rbx, rbp, r12 - r15. rsi and rdi will likely get clobbered
    push rbp    ; Start new call frame
    mov  rbp, rsp
    
    push rax
    push rcx
    push rdx
    push r8
    push r9
    push r10
    push r11
    cld         ; C code following sysV ABI requires DF be cleared on call
    call interrupt_handler
    ; popad     ; popad no valid op in 64-bit mode
    pop r11
    pop r10
    pop r9
    pop r8
    pop rcx
    pop rdx
    pop rax
    iret
