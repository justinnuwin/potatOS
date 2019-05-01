global isr_wrapper
global keyboard_isr_wrapper
extern interrupt_handler
extern keyboard_interrupt_handler
extern PIC_sendEOI
align  4

section .text
bits 64

; Follow SystemV ABI calling convention
isr_wrapper:
    push rbp    ; Start new stack frame
    mov  rbp, rsp
    
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14

    cld         ; C code following sysV ABI requires DF be cleared on call
    call interrupt_handler

    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    ; Call ROI here
    pop rax

    mov rsp, rbp    ; Remove stack frame
    pop rbp
    iretq

keyboard_isr_wrapper:
    push rbp    ; Start new stack frame
    mov  rbp, rsp
    
    push rbx
    push r12
    push r13
    push r14
    push r15

    cld         ; C code following sysV ABI requires DF be cleared on call
    call keyboard_interrupt_handler

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx

    mov rsp, rbp    ; Remove stack frame
    pop rbp
    mov rdi, 0x21
    call PIC_sendEOI
    iretq
