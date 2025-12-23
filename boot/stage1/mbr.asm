; ============================================================================
; mbr.asm - Master Boot Record (Stage 1 Bootloader)
; ============================================================================
; PURPOSE: First code executed by BIOS after POST. Must fit in 512 bytes.
;          Loads Stage 2 bootloader and transfers control to it.
;
; BIOS BEHAVIOR:
;   1. BIOS loads sector 0 of boot drive to 0x7C00
;   2. BIOS passes boot drive number in DL
;   3. Execution starts at 0x7C00
;
; MEMORY MAP (after MBR execution):
;   0x0000:0x0500 - 0x0000:0x7BFF : Free for bootloader use
;   0x0000:0x7C00 - 0x0000:0x7DFF : MBR (this code, 512 bytes)
;   0x0000:0x7E00 - onwards       : Stage 2 loader destination
;
; REGISTER CONVENTIONS:
;   DL = Boot drive number (preserved from BIOS, CRITICAL!)
;   DS:SI = Source pointer
;   ES:DI = Destination pointer
;
; LIMITATIONS:
;   - Maximum 510 bytes of code (2 bytes for boot signature)
;   - Must be position-independent or loaded at expected address
; ============================================================================

bits 16                         ; Real mode, 16-bit
org 0x7C00                      ; BIOS loads us here

; ============================================================================
; Constants
; ============================================================================
STAGE2_LOAD_ADDR    equ 0x7E00  ; Load Stage 2 right after MBR
STAGE2_SECTORS      equ 32      ; Number of sectors to load (16KB)
STACK_TOP           equ 0x7C00  ; Stack grows down from MBR

; ============================================================================
; Entry Point
; ============================================================================
start:
    ; --------------------------------------------------------------------
    ; Initialize segments and stack
    ; BIOS might leave segments in undefined state, so we set them up
    ; --------------------------------------------------------------------
    cli                         ; Disable interrupts during setup
    xor ax, ax                  ; AX = 0
    mov ds, ax                  ; DS = 0 (data segment)
    mov es, ax                  ; ES = 0 (extra segment)
    mov ss, ax                  ; SS = 0 (stack segment)
    mov sp, STACK_TOP           ; Stack pointer below MBR
    sti                         ; Re-enable interrupts

    ; --------------------------------------------------------------------
    ; Save boot drive number
    ; DL contains boot drive (0x00=floppy, 0x80=first HDD)
    ; This is CRITICAL - we need it for disk reads
    ; --------------------------------------------------------------------
    mov [boot_drive], dl

    ; --------------------------------------------------------------------
    ; Print welcome message
    ; --------------------------------------------------------------------
    mov si, msg_loading
    call print_string

    ; --------------------------------------------------------------------
    ; Reset disk system
    ; Some BIOSes need this before reading
    ; --------------------------------------------------------------------
    xor ax, ax                  ; AH=0: Reset disk
    mov dl, [boot_drive]
    int 0x13                    ; BIOS disk interrupt
    jc disk_error               ; Carry flag set = error

    ; --------------------------------------------------------------------
    ; Load Stage 2 bootloader
    ; Using BIOS INT 13h, AH=02h (Read Sectors)
    ;
    ; Parameters:
    ;   AH = 02h (read sectors)
    ;   AL = number of sectors to read
    ;   CH = cylinder number (0-based)
    ;   CL = sector number (1-based, sectors start at 1!)
    ;   DH = head number (0-based)
    ;   DL = drive number
    ;   ES:BX = destination buffer
    ; --------------------------------------------------------------------
    mov ah, 0x02                ; Read sectors function
    mov al, STAGE2_SECTORS      ; Number of sectors
    mov ch, 0                   ; Cylinder 0
    mov cl, 2                   ; Start from sector 2 (sector 1 is MBR)
    mov dh, 0                   ; Head 0
    mov dl, [boot_drive]        ; Boot drive
    mov bx, STAGE2_LOAD_ADDR    ; ES:BX = destination (ES=0 from earlier)
    int 0x13                    ; BIOS disk read
    jc disk_error               ; Jump if carry flag set (error)

    ; --------------------------------------------------------------------
    ; Verify sectors read
    ; AL should contain number of sectors actually read
    ; --------------------------------------------------------------------
    cmp al, STAGE2_SECTORS
    jne disk_error

    ; --------------------------------------------------------------------
    ; Success! Jump to Stage 2
    ; Pass boot drive in DL for Stage 2 to use
    ; --------------------------------------------------------------------
    mov si, msg_jumping
    call print_string

    mov dl, [boot_drive]        ; Pass boot drive to Stage 2
    jmp STAGE2_LOAD_ADDR        ; Transfer control to Stage 2

; ============================================================================
; Error Handler
; ============================================================================
disk_error:
    mov si, msg_disk_error
    call print_string
.halt:
    cli                         ; Disable interrupts
    hlt                         ; Halt CPU
    jmp .halt                   ; Loop forever if NMI wakes us

; ============================================================================
; print_string - Print null-terminated string via BIOS
; ============================================================================
; INPUT:  DS:SI = pointer to null-terminated string
; OUTPUT: None
; CLOBBERS: AX, BX, SI
; ============================================================================
print_string:
    mov ah, 0x0E                ; BIOS teletype function
    mov bh, 0                   ; Page 0
.loop:
    lodsb                       ; Load byte from [SI] into AL, increment SI
    test al, al                 ; Check if null terminator
    jz .done                    ; If zero, we're done
    int 0x10                    ; BIOS video interrupt
    jmp .loop                   ; Next character
.done:
    ret

; ============================================================================
; Data Section
; ============================================================================
boot_drive:     db 0            ; Storage for boot drive number

msg_loading:    db "Squirel: Loading...", 13, 10, 0
msg_jumping:    db "Squirel: Jumping to Stage 2", 13, 10, 0
msg_disk_error: db "Squirel: Disk error!", 13, 10, 0

; ============================================================================
; Boot Signature
; ============================================================================
; Pad to 510 bytes, then add boot signature
; The BIOS checks for 0x55, 0xAA at bytes 510-511 to identify bootable media
; ============================================================================
times 510 - ($ - $$) db 0       ; Pad with zeros
dw 0xAA55                       ; Boot signature (little-endian)
