; ============================================================================
; loader.asm - Stage 2 Bootloader
; ============================================================================
; PURPOSE: Second stage bootloader. Has more space than MBR (not limited to
;          512 bytes). Performs the CPU mode transitions:
;          16-bit Real Mode -> 32-bit Protected Mode -> 64-bit Long Mode
;          Then loads and jumps to the kernel.
;
; ENTRY CONDITIONS:
;   - Running in 16-bit real mode
;   - Loaded at 0x7E00 by Stage 1 (MBR)
;   - DL = boot drive number
;   - Interrupts enabled
;
; EXECUTION FLOW:
;   1. Enable A20 line (access memory above 1MB)
;   2. Load kernel from disk to 0x100000 (1MB mark)
;   3. Set up GDT for protected mode
;   4. Switch to 32-bit protected mode
;   5. Set up page tables for long mode
;   6. Switch to 64-bit long mode
;   7. Jump to kernel entry point
;
; MEMORY MAP:
;   0x00007E00 - Stage 2 code (this file)
;   0x00010000 - Temporary kernel load buffer
;   0x00100000 - Final kernel location (1MB)
; ============================================================================

bits 16
org 0x7E00

; ============================================================================
; Constants
; ============================================================================
KERNEL_LOAD_SEG     equ 0x1000      ; Segment for temp kernel load (0x10000)
KERNEL_LOAD_OFF     equ 0x0000      ; Offset
KERNEL_FINAL_ADDR   equ 0x100000    ; 1MB - final kernel location
KERNEL_SECTORS      equ 128         ; 64KB of kernel max
KERNEL_START_SECTOR equ 18          ; Sector after bootloader (BIOS 1-indexed: MBR=1, Stage2=2-17, Kernel=18+)

; Page table locations (must be 4KB aligned)
PML4_ADDR           equ 0x1000      ; Page Map Level 4
PDPT_ADDR           equ 0x2000      ; Page Directory Pointer Table
PD_ADDR             equ 0x3000      ; Page Directory
PT_ADDR             equ 0x4000      ; Page Table

; ============================================================================
; Entry Point (16-bit Real Mode)
; ============================================================================
stage2_start:
    ; Save boot drive
    mov [boot_drive], dl

    ; Print stage 2 start message
    mov si, msg_stage2
    call print_string_16

    ; --------------------------------------------------------------------
    ; Step 1: Enable A20 Line
    ; --------------------------------------------------------------------
    ; The A20 line is the 21st address line. In real mode, addresses wrap
    ; around at 1MB. We need to enable A20 to access memory above 1MB.
    ; --------------------------------------------------------------------
    call enable_a20
    mov si, msg_a20_ok
    call print_string_16

    ; --------------------------------------------------------------------
    ; Step 2: Load Kernel from Disk
    ; --------------------------------------------------------------------
    ; We load kernel to a low memory buffer first, then copy to 1MB
    ; after entering protected mode (can't access >1MB in real mode easily)
    ; --------------------------------------------------------------------
    mov si, msg_loading_kernel
    call print_string_16

    ; Load kernel using BIOS INT 13h (same as MBR)
    mov ax, KERNEL_LOAD_SEG
    mov es, ax                      ; ES = 0x1000
    xor bx, bx                      ; BX = 0, so ES:BX = 0x10000
    mov ah, 0x02                    ; Read sectors
    mov al, KERNEL_SECTORS          ; Number of sectors
    mov ch, 0                       ; Cylinder 0
    mov cl, KERNEL_START_SECTOR     ; Starting sector
    mov dh, 0                       ; Head 0
    mov dl, [boot_drive]            ; Drive
    int 0x13
    jc disk_error_16

    mov si, msg_kernel_loaded
    call print_string_16

    ; --------------------------------------------------------------------
    ; Step 3: Enter Protected Mode
    ; --------------------------------------------------------------------
    mov si, msg_pmode
    call print_string_16

    cli                             ; Disable interrupts

    ; Load GDT
    lgdt [gdt_descriptor]

    ; Set PE (Protection Enable) bit in CR0
    mov eax, cr0
    or eax, 1                       ; Set bit 0 (PE)
    mov cr0, eax

    ; Far jump to 32-bit code (flushes prefetch queue)
    jmp 0x08:protected_mode_start

; ============================================================================
; Enable A20 Line
; ============================================================================
; Uses multiple methods for compatibility:
;   1. BIOS INT 15h method (most reliable)
;   2. Keyboard controller method (fallback)
;   3. Fast A20 gate (if available)
; ============================================================================
enable_a20:
    ; Method 1: BIOS INT 15h, AX=2401h
    mov ax, 0x2401
    int 0x15
    jnc .done                       ; Success if CF=0

    ; Method 2: Keyboard controller
    call .wait_input
    mov al, 0xAD                    ; Disable keyboard
    out 0x64, al

    call .wait_input
    mov al, 0xD0                    ; Read output port
    out 0x64, al

    call .wait_output
    in al, 0x60                     ; Read output port data
    push ax

    call .wait_input
    mov al, 0xD1                    ; Write output port
    out 0x64, al

    call .wait_input
    pop ax
    or al, 2                        ; Set A20 bit
    out 0x60, al

    call .wait_input
    mov al, 0xAE                    ; Enable keyboard
    out 0x64, al

    call .wait_input

.done:
    ret

.wait_input:
    in al, 0x64
    test al, 2
    jnz .wait_input
    ret

.wait_output:
    in al, 0x64
    test al, 1
    jz .wait_output
    ret

; ============================================================================
; 16-bit Helper Functions
; ============================================================================
print_string_16:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

disk_error_16:
    mov si, msg_disk_error
    call print_string_16
.halt:
    cli
    hlt
    jmp .halt

; ============================================================================
; 16-bit Data
; ============================================================================
boot_drive:         db 0

msg_stage2:         db "Stage 2: Starting...", 13, 10, 0
msg_a20_ok:         db "Stage 2: A20 enabled", 13, 10, 0
msg_loading_kernel: db "Stage 2: Loading kernel...", 13, 10, 0
msg_kernel_loaded:  db "Stage 2: Kernel loaded", 13, 10, 0
msg_pmode:          db "Stage 2: Entering protected mode...", 13, 10, 0
msg_disk_error:     db "Stage 2: Disk error!", 13, 10, 0

; ============================================================================
; GDT (Global Descriptor Table)
; ============================================================================
; The GDT defines memory segments. In long mode, segmentation is mostly
; disabled, but we still need valid descriptors for the mode transitions.
;
; Each descriptor is 8 bytes:
;   Bytes 0-1: Limit (bits 0-15)
;   Bytes 2-3: Base (bits 0-15)
;   Byte 4:    Base (bits 16-23)
;   Byte 5:    Access byte
;   Byte 6:    Flags (4 bits) + Limit (bits 16-19)
;   Byte 7:    Base (bits 24-31)
; ============================================================================
align 8
gdt_start:
    ; Null descriptor (required)
    dq 0

gdt_code_32:
    ; 32-bit code segment
    ; Base=0, Limit=0xFFFFF, 4KB granularity, 32-bit, executable
    dw 0xFFFF           ; Limit (bits 0-15)
    dw 0                ; Base (bits 0-15)
    db 0                ; Base (bits 16-23)
    db 0x9A             ; Access: Present, Ring 0, Code, Executable, Readable
    db 0xCF             ; Flags: 4KB granularity, 32-bit + Limit (bits 16-19)
    db 0                ; Base (bits 24-31)

gdt_data_32:
    ; 32-bit data segment
    dw 0xFFFF
    dw 0
    db 0
    db 0x92             ; Access: Present, Ring 0, Data, Writable
    db 0xCF
    db 0

gdt_code_64:
    ; 64-bit code segment
    ; In long mode, base and limit are ignored
    dw 0                ; Limit (ignored)
    dw 0                ; Base (ignored)
    db 0                ; Base (ignored)
    db 0x9A             ; Access: Present, Ring 0, Code, Executable, Readable
    db 0xAF             ; Flags: Long mode + Limit (ignored)
    db 0                ; Base (ignored)

gdt_data_64:
    ; 64-bit data segment
    dw 0
    dw 0
    db 0
    db 0x92             ; Access: Present, Ring 0, Data, Writable
    db 0x00
    db 0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT - 1
    dd gdt_start                ; Address of GDT

; GDT segment selectors
CODE_SEG_32 equ gdt_code_32 - gdt_start     ; 0x08
DATA_SEG_32 equ gdt_data_32 - gdt_start     ; 0x10
CODE_SEG_64 equ gdt_code_64 - gdt_start     ; 0x18
DATA_SEG_64 equ gdt_data_64 - gdt_start     ; 0x20

; ============================================================================
; 32-bit Protected Mode Code
; ============================================================================
bits 32
protected_mode_start:
    ; --------------------------------------------------------------------
    ; Set up 32-bit segments
    ; --------------------------------------------------------------------
    mov ax, DATA_SEG_32
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000                ; Stack in free memory

    ; --------------------------------------------------------------------
    ; Copy kernel from 0x10000 to 0x100000 (1MB)
    ; Now that we're in protected mode, we can access above 1MB
    ; --------------------------------------------------------------------
    mov esi, 0x10000                ; Source: temporary load location
    mov edi, KERNEL_FINAL_ADDR      ; Destination: 1MB
    mov ecx, KERNEL_SECTORS * 512 / 4 ; Size in dwords
    rep movsd                       ; Copy!

    ; --------------------------------------------------------------------
    ; Set up paging for long mode
    ; We create identity mapping for first 2MB
    ; (virtual address = physical address)
    ; --------------------------------------------------------------------
    call setup_page_tables

    ; --------------------------------------------------------------------
    ; Enable PAE (Physical Address Extension)
    ; Required for long mode
    ; --------------------------------------------------------------------
    mov eax, cr4
    or eax, (1 << 5)                ; Set PAE bit
    mov cr4, eax

    ; --------------------------------------------------------------------
    ; Load PML4 address into CR3
    ; --------------------------------------------------------------------
    mov eax, PML4_ADDR
    mov cr3, eax

    ; --------------------------------------------------------------------
    ; Enable Long Mode in EFER MSR
    ; EFER (Extended Feature Enable Register) MSR number: 0xC0000080
    ; Bit 8 (LME) = Long Mode Enable
    ; --------------------------------------------------------------------
    mov ecx, 0xC0000080             ; EFER MSR
    rdmsr                           ; Read EFER into EDX:EAX
    or eax, (1 << 8)                ; Set LME bit
    wrmsr                           ; Write back

    ; --------------------------------------------------------------------
    ; Enable Paging (which activates long mode)
    ; --------------------------------------------------------------------
    mov eax, cr0
    or eax, (1 << 31)               ; Set PG bit (paging)
    mov cr0, eax

    ; --------------------------------------------------------------------
    ; Far jump to 64-bit code
    ; This activates long mode fully
    ; --------------------------------------------------------------------
    jmp CODE_SEG_64:long_mode_start

; ============================================================================
; setup_page_tables - Create identity-mapped page tables
; ============================================================================
; Creates a simple identity mapping for the first 2MB of memory.
; This means virtual address X maps to physical address X.
;
; Page table structure (4-level paging):
;   PML4 -> PDPT -> PD -> PT -> Physical Page
;
; We use 2MB pages to simplify (only need PML4, PDPT, PD)
; ============================================================================
setup_page_tables:
    ; Clear page table memory
    mov edi, PML4_ADDR
    mov ecx, 4096                   ; 4 pages * 4KB = 16KB
    xor eax, eax
    rep stosd

    ; PML4[0] -> PDPT
    mov dword [PML4_ADDR], PDPT_ADDR | 0x03     ; Present + Writable

    ; PDPT[0] -> PD
    mov dword [PDPT_ADDR], PD_ADDR | 0x03       ; Present + Writable

    ; PD[0] -> 2MB page at 0x000000 (using huge pages)
    mov dword [PD_ADDR], 0x000000 | 0x83        ; Present + Writable + Huge (2MB)

    ; PD[1] -> 2MB page at 0x200000 (covers kernel at 1MB)
    mov dword [PD_ADDR + 8], 0x200000 | 0x83    ; Present + Writable + Huge

    ret

; ============================================================================
; 64-bit Long Mode Code
; ============================================================================
bits 64
long_mode_start:
    ; --------------------------------------------------------------------
    ; Set up 64-bit segments
    ; --------------------------------------------------------------------
    mov ax, DATA_SEG_64
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up a proper stack
    mov rsp, 0x90000

    ; --------------------------------------------------------------------
    ; Jump to kernel!
    ; The kernel is loaded at 0x100000 (1MB)
    ; kernel_start is the entry point defined in kernel.ld
    ; --------------------------------------------------------------------
    mov rax, KERNEL_FINAL_ADDR
    jmp rax

    ; Should never reach here
.hang:
    cli
    hlt
    jmp .hang

; ============================================================================
; Padding
; ============================================================================
; Pad to fill sectors (Stage 2 size depends on how many sectors MBR loads)
times 8192 - ($ - $$) db 0
