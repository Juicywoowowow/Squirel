/**
 * @file gdt.c
 * @brief Global Descriptor Table management
 * 
 * The GDT defines memory segments. In 64-bit long mode, segmentation
 * is mostly ignored (flat memory model), but we still need valid
 * descriptors for the CPU to function.
 * 
 * GDT STRUCTURE:
 *   Entry 0: Null descriptor (required)
 *   Entry 1: Kernel code segment (64-bit)
 *   Entry 2: Kernel data segment (64-bit)
 *   Entry 3: User code segment (future)
 *   Entry 4: User data segment (future)
 *   Entry 5: TSS (Task State Segment, for interrupts)
 * 
 * NOTE: The bootloader already set up a GDT. This module provides
 *       a proper kernel GDT and allows runtime modifications (TSS).
 */

#include <squirel/types.h>
#include <lib/memory/memory.h>

/* ============================================================================
 * GDT Entry Structure
 * ============================================================================ */

/**
 * @brief GDT entry (8 bytes)
 * 
 * Format is complex due to historical x86 design.
 */
typedef struct PACKED {
    uint16_t limit_low;      /* Limit bits 0-15 */
    uint16_t base_low;       /* Base bits 0-15 */
    uint8_t  base_middle;    /* Base bits 16-23 */
    uint8_t  access;         /* Access byte */
    uint8_t  granularity;    /* Limit bits 16-19, flags */
    uint8_t  base_high;      /* Base bits 24-31 */
} gdt_entry_t;

/**
 * @brief GDT pointer (for LGDT instruction)
 */
typedef struct PACKED {
    uint16_t limit;          /* Size of GDT - 1 */
    uint64_t base;           /* Address of GDT */
} gdt_ptr_t;

/* ============================================================================
 * GDT Data
 * ============================================================================ */

#define GDT_ENTRIES 6

static gdt_entry_t gdt[GDT_ENTRIES];
static gdt_ptr_t gdt_ptr;

/* ============================================================================
 * Private Functions
 * ============================================================================ */

/**
 * @brief Set a GDT entry
 */
static void gdt_set_entry(int index, uint32_t base, uint32_t limit, 
                          uint8_t access, uint8_t gran) {
    gdt[index].base_low    = base & 0xFFFF;
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high   = (base >> 24) & 0xFF;
    gdt[index].limit_low   = limit & 0xFFFF;
    gdt[index].granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[index].access      = access;
}

/* ============================================================================
 * Public Functions
 * ============================================================================ */

/**
 * @brief Initialize the GDT
 * 
 * Sets up kernel code/data segments for long mode.
 */
void gdt_init(void) {
    /* Clear GDT */
    memset(gdt, 0, sizeof(gdt));
    
    /* Null descriptor (index 0) */
    gdt_set_entry(0, 0, 0, 0, 0);
    
    /* 
     * Kernel code segment (index 1, selector 0x08)
     * Access: Present, Ring 0, Code, Executable, Readable
     * Granularity: Long mode (bit 5)
     */
    gdt_set_entry(1, 0, 0xFFFFF, 0x9A, 0xA0);
    
    /* 
     * Kernel data segment (index 2, selector 0x10)
     * Access: Present, Ring 0, Data, Writable
     */
    gdt_set_entry(2, 0, 0xFFFFF, 0x92, 0xC0);
    
    /* User code segment (index 3, selector 0x18 | 3 = 0x1B) - future */
    gdt_set_entry(3, 0, 0xFFFFF, 0xFA, 0xA0);
    
    /* User data segment (index 4, selector 0x20 | 3 = 0x23) - future */
    gdt_set_entry(4, 0, 0xFFFFF, 0xF2, 0xC0);
    
    /* TSS will be set up later */
    
    /* Set up GDT pointer */
    gdt_ptr.limit = sizeof(gdt) - 1;
    gdt_ptr.base = (uint64_t)&gdt;
    
    /* Load GDT */
    __asm__ volatile (
        "lgdt %0\n"
        "pushq $0x08\n"          /* Push code segment selector */
        "leaq 1f(%%rip), %%rax\n" /* Get address of label 1 */
        "pushq %%rax\n"
        "lretq\n"                /* Far return to reload CS */
        "1:\n"
        "movw $0x10, %%ax\n"     /* Data segment selector */
        "movw %%ax, %%ds\n"
        "movw %%ax, %%es\n"
        "movw %%ax, %%fs\n"
        "movw %%ax, %%gs\n"
        "movw %%ax, %%ss\n"
        :
        : "m"(gdt_ptr)
        : "rax", "memory"
    );
}
