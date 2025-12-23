/* ============================================================================
 * kernel.ld - Kernel Linker Script
 * ============================================================================
 * PURPOSE: Controls memory layout of the kernel binary.
 *          The kernel is loaded at 1MB (0x100000) by the bootloader.
 *
 * MEMORY MAP:
 *   0x00000000 - 0x000FFFFF : Reserved (real mode, BIOS, etc.)
 *   0x00100000 - ...        : Kernel code and data
 *
 * SECTIONS:
 *   .text   : Executable code
 *   .rodata : Read-only data (strings, constants)
 *   .data   : Initialized read-write data
 *   .bss    : Uninitialized data (zeroed by kernel)
 * ============================================================================
 */

ENTRY(kernel_start)

SECTIONS
{
    /* Kernel loaded at 1MB mark */
    . = 0x100000;

    /* Keep track of kernel start */
    __kernel_start = .;

    /* Code section */
    .text ALIGN(4K) :
    {
        *(.text.boot)    /* Boot code first (kernel entry) */
        *(.text)         /* All other code */
        *(.text.*)
    }

    /* Read-only data */
    .rodata ALIGN(4K) :
    {
        *(.rodata)
        *(.rodata.*)
    }

    /* Initialized data */
    .data ALIGN(4K) :
    {
        *(.data)
        *(.data.*)
    }

    /* Uninitialized data (BSS) */
    .bss ALIGN(4K) :
    {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end = .;
    }

    /* Kernel end marker */
    __kernel_end = .;

    /* Discard unnecessary sections */
    /DISCARD/ :
    {
        *(.comment)
        *(.note.*)
        *(.eh_frame)
        *(.eh_frame_hdr)
    }
}
