; ============================================================================
; start64.asm - 64-bit Kernel Entry Point
; ============================================================================
; PURPOSE: First kernel code executed after bootloader transitions to long mode.
;          Sets up a clean 64-bit environment and calls the C kernel main.
;
; ENTRY CONDITIONS:
;   - 64-bit long mode active
;   - Paging enabled with identity mapping
;   - Interrupts disabled
;   - Stack set up by bootloader
;
; RESPONSIBILITIES:
;   1. Clear BSS section (uninitialized data)
;   2. Set up a clean stack
;   3. Call kmain() - never returns
;   4. Halt if kmain() somehow returns
; ============================================================================

bits 64
section .text.boot

; Export symbols
global kernel_start
extern kmain
extern __bss_start
extern __bss_end

; ============================================================================
; Kernel Entry Point
; ============================================================================
kernel_start:
    ; --------------------------------------------------------------------
    ; Disable interrupts (should already be disabled, but be safe)
    ; We'll enable them after setting up IDT
    ; --------------------------------------------------------------------
    cli

    ; --------------------------------------------------------------------
    ; Clear BSS Section
    ; BSS contains uninitialized global/static variables
    ; C expects these to be zero-initialized
    ; --------------------------------------------------------------------
    mov rdi, __bss_start            ; Start of BSS
    mov rcx, __bss_end              ; End of BSS
    sub rcx, rdi                    ; Size = end - start
    shr rcx, 3                      ; Convert to qwords (divide by 8)
    xor rax, rax                    ; Value to store (0)
    rep stosq                       ; Fill with zeros

    ; --------------------------------------------------------------------
    ; Set up stack
    ; We use memory below 0x90000 for the kernel stack
    ; Stack grows downward, so SP points to top of usable area
    ; --------------------------------------------------------------------
    mov rsp, 0x90000                ; 64KB of stack space
    mov rbp, rsp                    ; Frame pointer

    ; --------------------------------------------------------------------
    ; Call C kernel main
    ; The System V AMD64 ABI is used:
    ;   - First 6 integer args: RDI, RSI, RDX, RCX, R8, R9
    ;   - Return value in RAX
    ;   - Stack must be 16-byte aligned before CALL
    ; --------------------------------------------------------------------
    ; Ensure 16-byte stack alignment (required by ABI)
    and rsp, ~0xF

    ; Call kmain() - this should never return
    call kmain

    ; --------------------------------------------------------------------
    ; Halt Loop
    ; If kmain() returns (it shouldn't), halt the CPU forever
    ; --------------------------------------------------------------------
.halt:
    cli                             ; Disable interrupts
    hlt                             ; Halt CPU until interrupt (which won't come)
    jmp .halt                       ; Just in case NMI wakes us up
