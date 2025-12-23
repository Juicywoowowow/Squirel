; ============================================================================
; interrupts.asm - Low-level Interrupt Service Routine Stubs
; ============================================================================
; PURPOSE: Provides assembly entry points for CPU exceptions.
;          These stubs save registers, call the C handler, then restore.
;
; CALLING CONVENTION:
;   When an exception occurs, the CPU pushes:
;     - SS, RSP (if privilege change)
;     - RFLAGS
;     - CS, RIP
;     - Error code (some exceptions only)
;
;   Our stubs:
;     - Push a dummy error code if CPU didn't
;     - Push exception number
;     - Jump to common handler
;     - Common handler saves all registers
;     - Calls C function exception_handler(vector, error_code)
;     - Restores registers
;     - Returns with IRETQ
;
; REGISTER PRESERVATION:
;   The System V AMD64 ABI specifies:
;     Caller-saved (we can clobber): RAX, RCX, RDX, RSI, RDI, R8-R11
;     Callee-saved (must preserve): RBX, RBP, R12-R15
;   We save ALL registers for safety in exception context.
; ============================================================================

bits 64
section .text

; External C handler
extern exception_handler

; ============================================================================
; Macro: ISR stub without error code
; ============================================================================
%macro ISR_NOERR 1
global isr_stub_%1
isr_stub_%1:
    push 0                      ; Push dummy error code
    push %1                     ; Push exception number
    jmp isr_common
%endmacro

; ============================================================================
; Macro: ISR stub with error code (CPU pushes it)
; ============================================================================
%macro ISR_ERR 1
global isr_stub_%1
isr_stub_%1:
    ; Error code already pushed by CPU
    push %1                     ; Push exception number
    jmp isr_common
%endmacro

; ============================================================================
; Exception Stubs (0-21)
; ============================================================================

ISR_NOERR 0     ; Division by Zero
ISR_NOERR 1     ; Debug
ISR_NOERR 2     ; NMI
ISR_NOERR 3     ; Breakpoint
ISR_NOERR 4     ; Overflow
ISR_NOERR 5     ; Bound Range Exceeded
ISR_NOERR 6     ; Invalid Opcode
ISR_NOERR 7     ; Device Not Available
ISR_ERR   8     ; Double Fault
ISR_NOERR 9     ; Coprocessor Segment Overrun
ISR_ERR   10    ; Invalid TSS
ISR_ERR   11    ; Segment Not Present
ISR_ERR   12    ; Stack-Segment Fault
ISR_ERR   13    ; General Protection Fault
ISR_ERR   14    ; Page Fault
ISR_NOERR 15    ; Reserved
ISR_NOERR 16    ; x87 FPU Error
ISR_ERR   17    ; Alignment Check
ISR_NOERR 18    ; Machine Check
ISR_NOERR 19    ; SIMD Exception
ISR_NOERR 20    ; Virtualization Exception
ISR_ERR   21    ; Control Protection Exception

; ============================================================================
; Common ISR Handler
; ============================================================================
; Stack at this point:
;   [RSP+8]  = error code (or 0)
;   [RSP+0]  = exception number
;   ... (RIP, CS, RFLAGS, RSP, SS pushed by CPU)
; ============================================================================
isr_common:
    ; Save all general-purpose registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Load exception number and error code for C handler
    ; After pushes: vector is at [RSP + 120], error at [RSP + 128]
    mov rdi, [rsp + 120]        ; First arg: exception number
    mov rsi, [rsp + 128]        ; Second arg: error code

    ; Call C handler
    call exception_handler

    ; Handler should not return, but just in case...
    ; Restore registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; Remove exception number and error code from stack
    add rsp, 16

    ; Return from interrupt
    iretq
