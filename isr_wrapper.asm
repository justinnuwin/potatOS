global keyboard_isr_wrapper
extern generic_interrupt_handler
extern generic_exception_handler
extern keyboard_interrupt_handler
extern PIC_sendEOI
align  4

%macro setup_isr 0
    push rbp    ; Start new stack frame
    mov  rbp, rsp
    
    push rbx
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro cleanup_isr 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx

    mov rsp, rbp    ; Remove stack frame
    pop rbp
%endmacro

section .text
bits 64

keyboard_isr_wrapper:
    setup_isr
    cld         ; C code following sysV ABI requires DF be cleared on call
    call keyboard_interrupt_handler
    cleanup_isr
    mov rdi, 0x21
    call PIC_sendEOI
    iretq

;
; Declare generic interrupt handlers
;

%macro exception_no_err 1
    global isr_wrapper_%1
    isr_wrapper_%1:
        setup_isr
        cld         ; C code following sysV ABI requires DF be cleared on call
        mov rdi, %1
        mov rsi, 0
        call generic_exception_handler
        cleanup_isr
        mov rdi, %1
        call PIC_sendEOI
        iretq
%endmacro

%macro exception_err 1
    global isr_wrapper_%1
    isr_wrapper_%1:
        setup_isr
        cld         ; C code following sysV ABI requires DF be cleared on call
        mov rdi, %1
        pop rsi
        call generic_exception_handler
        cleanup_isr
        mov rdi, %1
        call PIC_sendEOI
        iretq
%endmacro

; Follow SystemV ABI calling convention
%macro isr 1
    global isr_wrapper_%1
    isr_wrapper_%1:
        push rbp    ; Start new stack frame
        mov  rbp, rsp
        
        push rbx
        push r12
        push r13
        push r14
        push r15

        cld         ; C code following sysV ABI requires DF be cleared on call
        mov rdi, %1
        call generic_interrupt_handler

        pop r15
        pop r14
        pop r13
        pop r12
        pop rbx

        mov rsp, rbp    ; Remove stack frame
        pop rbp
        mov rdi, %1
        call PIC_sendEOI
        iretq
%endmacro

exception_no_err 0x0
exception_no_err 0x1
exception_no_err 0x2
exception_no_err 0x3
exception_no_err 0x4
exception_no_err 0x5
exception_no_err 0x6
exception_no_err 0x7
exception_no_err 0x8
exception_no_err 0x9
exception_err 0xa
exception_err 0xb
exception_err 0xc
exception_err 0xd
exception_err 0xe
exception_no_err 0xf
exception_no_err 0x10
exception_err 0x11
exception_no_err 0x12
exception_no_err 0x13
exception_no_err 0x14
exception_no_err 0x15
exception_no_err 0x16
exception_no_err 0x17
exception_no_err 0x18
exception_no_err 0x19
exception_no_err 0x1a
exception_no_err 0x1b
exception_no_err 0x1c
exception_no_err 0x1d
exception_err 0x1e
exception_no_err 0x1f

isr 0x20
isr 0x21
isr 0x22
isr 0x23
isr 0x24
isr 0x25
isr 0x26
isr 0x27
isr 0x28
isr 0x29
isr 0x2a
isr 0x2b
isr 0x2c
isr 0x2d
isr 0x2e
isr 0x2f

isr 0x30
isr 0x31
isr 0x32
isr 0x33
isr 0x34
isr 0x35
isr 0x36
isr 0x37
isr 0x38
isr 0x39
isr 0x3a
isr 0x3b
isr 0x3c
isr 0x3d
isr 0x3e
isr 0x3f

isr 0x40
isr 0x41
isr 0x42
isr 0x43
isr 0x44
isr 0x45
isr 0x46
isr 0x47
isr 0x48
isr 0x49
isr 0x4a
isr 0x4b
isr 0x4c
isr 0x4d
isr 0x4e
isr 0x4f

isr 0x50
isr 0x51
isr 0x52
isr 0x53
isr 0x54
isr 0x55
isr 0x56
isr 0x57
isr 0x58
isr 0x59
isr 0x5a
isr 0x5b
isr 0x5c
isr 0x5d
isr 0x5e
isr 0x5f

isr 0x60
isr 0x61
isr 0x62
isr 0x63
isr 0x64
isr 0x65
isr 0x66
isr 0x67
isr 0x68
isr 0x69
isr 0x6a
isr 0x6b
isr 0x6c
isr 0x6d
isr 0x6e
isr 0x6f

isr 0x70
isr 0x71
isr 0x72
isr 0x73
isr 0x74
isr 0x75
isr 0x76
isr 0x77
isr 0x78
isr 0x79
isr 0x7a
isr 0x7b
isr 0x7c
isr 0x7d
isr 0x7e
isr 0x7f

isr 0x80
isr 0x81
isr 0x82
isr 0x83
isr 0x84
isr 0x85
isr 0x86
isr 0x87
isr 0x88
isr 0x89
isr 0x8a
isr 0x8b
isr 0x8c
isr 0x8d
isr 0x8e
isr 0x8f

isr 0x90
isr 0x91
isr 0x92
isr 0x93
isr 0x94
isr 0x95
isr 0x96
isr 0x97
isr 0x98
isr 0x99
isr 0x9a
isr 0x9b
isr 0x9c
isr 0x9d
isr 0x9e
isr 0x9f
