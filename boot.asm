global start

section .text
bits 32
start:
    mov esp, stack_top

    ; Checks and paging are written using code from
    ; https://os.phil-opp.com/entering-longmode/
    ; https://wiki.osdev.org/Setting_Up_Long_Mode#x86_or_x86-64
    call check_multiboot
    call check_cpuid
    call check_long_mode
    call setup_page_tables
    call enable_paging
    
    ; print `OK` to screen
    mov dword [0xb8000], 0x2f4b2f4f
    hlt

check_multiboot:
    cmp eax, 0x36d76289
    jne .no_multiboot
    ret
.no_multiboot:
    mov al, "0"
    jmp error

check_cpuid:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je .no_cpuid
    ret
.no_cpuid:
    mov al, "1"
    jmp error 

check_long_mode:
    mov eax, 0x80000000
    cpuid              
    cmp eax, 0x80000001
    jb .no_long_mode   
    mov eax, 0x80000001
    cpuid              
    test edx, 1 << 29  
    jz .no_long_mode   
    ret
.no_long_mode:
    mov al, "2"
    jmp error

setup_page_tables:
    ; Map first P4 entry to P3 table
    mov eax, p3_table
    or eax, 0b11    ; present + writable
    mov [p4_table], eax
    
    ; Map first P3 entry to P2 table
    mov eax, p2_table
    or eax, 0b11    ; present + writable
    mov [p3_table], eax

    ; map each P2 entry to a huge 2MiB page
    mov ecx, 0
.map_p2_table:
    ; map ecx-th P2 entry to a huge page that starts at address 2MiB*ecx
    mov eax, 0x200000               ; 2MiB
    mul ecx                         ; start address of ecx-th page
    or eax, 0b10000011              ; present + writable + huge
    mov [p2_table + ecx * 8], eax   ; map ecx-th entry
    inc ecx
    cmp ecx, 512
    jne .map_p2_table

    ret

enable_paging:
    ; load P4 to cr3 register (cpu uses this to access the P4 table)
    mov eax, p4_table
    mov cr3, eax

    ; enable PAE-flag in cr4 (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; set the long mode bit in the EFER MSR (model specific register)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging in the cr0 register
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ret

; Prints `ERR: ` and the given error code to the screen an d hangs.
; parameter: error code (in ascii) in al
error:
    mov dword [0xb8000], 0x4f524f45
    mov dword [0xb8004], 0x4f3a4f52
    mov dword [0xb8008], 0x4f204f20
    mov byte  [0xb800a], al
    hlt

section .bss
align 4096
p4_table:       ; Page-Map Level-4 Table (PML4)
    resb 4096
p3_table:       ; Page-Directory Pointer Table (PDP)
    resb 4096
p2_table:       ; Page-Directory Table (PD)
    resb 4096
p1_table:       ; Page Table (PT)
    resb 4096
    
stack_bottom:
    resb 64
stack_top:
