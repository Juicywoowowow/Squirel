/**
 * @file idt.c
 * @brief Interrupt Descriptor Table management
 * 
 * The IDT defines handlers for interrupts and exceptions.
 * This is a minimal implementation that sets up basic exception handlers.
 * 
 * INTERRUPT TYPES:
 *   0-31:   CPU exceptions (divide by zero, page fault, etc.)
 *   32-255: External interrupts (IRQs from PIC/APIC)
 * 
 * NOTE: For now, we don't use interrupts - keyboard uses polling.
 *       This is just a stub to set up valid exception handlers.
 */

#include <squirel/types.h>
#include <lib/memory/memory.h>
#include <lib/printf/printf.h>
#include <drivers/vga/vga_text.h>

/* ============================================================================
 * IDT Entry Structure
 * ============================================================================ */

/**
 * @brief IDT entry for 64-bit mode (16 bytes)
 */
typedef struct PACKED {
    uint16_t offset_low;     /* Offset bits 0-15 */
    uint16_t selector;       /* Code segment selector */
    uint8_t  ist;            /* Interrupt Stack Table offset */
    uint8_t  type_attr;      /* Type and attributes */
    uint16_t offset_mid;     /* Offset bits 16-31 */
    uint32_t offset_high;    /* Offset bits 32-63 */
    uint32_t reserved;       /* Reserved, must be 0 */
} idt_entry_t;

/**
 * @brief IDT pointer (for LIDT instruction)
 */
typedef struct PACKED {
    uint16_t limit;
    uint64_t base;
} idt_ptr_t;

/* ============================================================================
 * IDT Data
 * ============================================================================ */

#define IDT_ENTRIES 256

static idt_entry_t idt[IDT_ENTRIES];
static idt_ptr_t idt_ptr;

/* Exception names for debugging */
static const char *exception_names[] = {
    "Division by Zero",          /* 0 */
    "Debug",                     /* 1 */
    "Non-Maskable Interrupt",    /* 2 */
    "Breakpoint",                /* 3 */
    "Overflow",                  /* 4 */
    "Bound Range Exceeded",      /* 5 */
    "Invalid Opcode",            /* 6 */
    "Device Not Available",      /* 7 */
    "Double Fault",              /* 8 */
    "Coprocessor Segment Overrun", /* 9 */
    "Invalid TSS",               /* 10 */
    "Segment Not Present",       /* 11 */
    "Stack-Segment Fault",       /* 12 */
    "General Protection Fault",  /* 13 */
    "Page Fault",                /* 14 */
    "Reserved",                  /* 15 */
    "x87 FPU Error",             /* 16 */
    "Alignment Check",           /* 17 */
    "Machine Check",             /* 18 */
    "SIMD Exception",            /* 19 */
    "Virtualization Exception",  /* 20 */
    "Control Protection Exception", /* 21 */
};

/* ============================================================================
 * Private Functions
 * ============================================================================ */

/**
 * @brief Set an IDT entry
 */
static void idt_set_entry(int index, uint64_t handler, uint16_t selector, 
                          uint8_t ist, uint8_t type_attr) {
    idt[index].offset_low  = handler & 0xFFFF;
    idt[index].offset_mid  = (handler >> 16) & 0xFFFF;
    idt[index].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[index].selector    = selector;
    idt[index].ist         = ist;
    idt[index].type_attr   = type_attr;
    idt[index].reserved    = 0;
}

/**
 * @brief Default exception handler (C part)
 * 
 * Called by the assembly stubs when an exception occurs.
 */
void exception_handler(uint64_t vector, uint64_t error_code) {
    vga_set_color(VGA_WHITE, VGA_RED);
    vga_clear();
    
    kprintf("\n\n");
    kprintf("  *** KERNEL PANIC ***\n\n");
    
    const char *name = (vector < 22) ? exception_names[vector] : "Unknown";
    kprintf("  Exception: %s (#%d)\n", name, (int)vector);
    kprintf("  Error Code: 0x%016llX\n", error_code);
    kprintf("\n");
    kprintf("  System halted.\n");
    
    /* Halt forever */
    for (;;) {
        __asm__ volatile("cli; hlt");
    }
}

/* ============================================================================
 * Assembly Stubs (defined in interrupts.asm)
 * ============================================================================ */

/* These are defined in interrupts.asm */
extern void isr_stub_0(void);
extern void isr_stub_1(void);
extern void isr_stub_2(void);
extern void isr_stub_3(void);
extern void isr_stub_4(void);
extern void isr_stub_5(void);
extern void isr_stub_6(void);
extern void isr_stub_7(void);
extern void isr_stub_8(void);
extern void isr_stub_9(void);
extern void isr_stub_10(void);
extern void isr_stub_11(void);
extern void isr_stub_12(void);
extern void isr_stub_13(void);
extern void isr_stub_14(void);
extern void isr_stub_15(void);
extern void isr_stub_16(void);
extern void isr_stub_17(void);
extern void isr_stub_18(void);
extern void isr_stub_19(void);
extern void isr_stub_20(void);
extern void isr_stub_21(void);

/* ============================================================================
 * Public Functions
 * ============================================================================ */

/**
 * @brief Initialize the IDT
 */
void idt_init(void) {
    /* Clear IDT */
    memset(idt, 0, sizeof(idt));
    
    /* 
     * Type attribute for interrupt gates:
     * Bit 7: Present (1)
     * Bits 5-6: DPL (0 = ring 0)
     * Bit 4: 0 (fixed)
     * Bits 0-3: Gate type (0xE = 64-bit interrupt gate)
     */
    uint8_t int_gate = 0x8E;  /* Present, Ring 0, Interrupt Gate */
    
    /* Set up exception handlers (0-21) */
    idt_set_entry(0,  (uint64_t)isr_stub_0,  0x08, 0, int_gate);
    idt_set_entry(1,  (uint64_t)isr_stub_1,  0x08, 0, int_gate);
    idt_set_entry(2,  (uint64_t)isr_stub_2,  0x08, 0, int_gate);
    idt_set_entry(3,  (uint64_t)isr_stub_3,  0x08, 0, int_gate);
    idt_set_entry(4,  (uint64_t)isr_stub_4,  0x08, 0, int_gate);
    idt_set_entry(5,  (uint64_t)isr_stub_5,  0x08, 0, int_gate);
    idt_set_entry(6,  (uint64_t)isr_stub_6,  0x08, 0, int_gate);
    idt_set_entry(7,  (uint64_t)isr_stub_7,  0x08, 0, int_gate);
    idt_set_entry(8,  (uint64_t)isr_stub_8,  0x08, 0, int_gate);
    idt_set_entry(9,  (uint64_t)isr_stub_9,  0x08, 0, int_gate);
    idt_set_entry(10, (uint64_t)isr_stub_10, 0x08, 0, int_gate);
    idt_set_entry(11, (uint64_t)isr_stub_11, 0x08, 0, int_gate);
    idt_set_entry(12, (uint64_t)isr_stub_12, 0x08, 0, int_gate);
    idt_set_entry(13, (uint64_t)isr_stub_13, 0x08, 0, int_gate);
    idt_set_entry(14, (uint64_t)isr_stub_14, 0x08, 0, int_gate);
    idt_set_entry(15, (uint64_t)isr_stub_15, 0x08, 0, int_gate);
    idt_set_entry(16, (uint64_t)isr_stub_16, 0x08, 0, int_gate);
    idt_set_entry(17, (uint64_t)isr_stub_17, 0x08, 0, int_gate);
    idt_set_entry(18, (uint64_t)isr_stub_18, 0x08, 0, int_gate);
    idt_set_entry(19, (uint64_t)isr_stub_19, 0x08, 0, int_gate);
    idt_set_entry(20, (uint64_t)isr_stub_20, 0x08, 0, int_gate);
    idt_set_entry(21, (uint64_t)isr_stub_21, 0x08, 0, int_gate);
    
    /* Set up IDT pointer and load IDT */
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)&idt;
    
    __asm__ volatile("lidt %0" : : "m"(idt_ptr));
}
