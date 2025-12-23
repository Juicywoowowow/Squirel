/**
 * @file x86_64.h
 * @brief x86_64 architecture-specific definitions
 * 
 * Provides architecture-specific types, macros, and inline functions.
 */

#ifndef _ARCH_X86_64_H
#define _ARCH_X86_64_H

#include <squirel/types.h>

/* ============================================================================
 * CPU Control
 * ============================================================================ */

/**
 * @brief Halt the CPU until the next interrupt
 */
static ALWAYS_INLINE void halt(void) {
    __asm__ volatile("hlt");
}

/**
 * @brief Disable interrupts
 */
static ALWAYS_INLINE void cli(void) {
    __asm__ volatile("cli");
}

/**
 * @brief Enable interrupts
 */
static ALWAYS_INLINE void sti(void) {
    __asm__ volatile("sti");
}

/**
 * @brief Disable interrupts and halt (for panic situations)
 */
static ALWAYS_INLINE NORETURN void hang(void) {
    for (;;) {
        __asm__ volatile("cli; hlt");
    }
}

/* ============================================================================
 * Control Registers
 * ============================================================================ */

/**
 * @brief Read CR0 register
 */
static ALWAYS_INLINE uint64_t read_cr0(void) {
    uint64_t val;
    __asm__ volatile("mov %%cr0, %0" : "=r"(val));
    return val;
}

/**
 * @brief Write CR0 register
 */
static ALWAYS_INLINE void write_cr0(uint64_t val) {
    __asm__ volatile("mov %0, %%cr0" : : "r"(val));
}

/**
 * @brief Read CR2 register (page fault address)
 */
static ALWAYS_INLINE uint64_t read_cr2(void) {
    uint64_t val;
    __asm__ volatile("mov %%cr2, %0" : "=r"(val));
    return val;
}

/**
 * @brief Read CR3 register (page table base)
 */
static ALWAYS_INLINE uint64_t read_cr3(void) {
    uint64_t val;
    __asm__ volatile("mov %%cr3, %0" : "=r"(val));
    return val;
}

/**
 * @brief Write CR3 register (flushes TLB)
 */
static ALWAYS_INLINE void write_cr3(uint64_t val) {
    __asm__ volatile("mov %0, %%cr3" : : "r"(val));
}

/**
 * @brief Read CR4 register
 */
static ALWAYS_INLINE uint64_t read_cr4(void) {
    uint64_t val;
    __asm__ volatile("mov %%cr4, %0" : "=r"(val));
    return val;
}

/* ============================================================================
 * MSR (Model-Specific Registers)
 * ============================================================================ */

/**
 * @brief Read a Model-Specific Register
 * 
 * @param msr  MSR number
 * @return     64-bit value
 */
static ALWAYS_INLINE uint64_t read_msr(uint32_t msr) {
    uint32_t low, high;
    __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
    return ((uint64_t)high << 32) | low;
}

/**
 * @brief Write a Model-Specific Register
 * 
 * @param msr    MSR number
 * @param value  Value to write
 */
static ALWAYS_INLINE void write_msr(uint32_t msr, uint64_t value) {
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    __asm__ volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

/* ============================================================================
 * CPUID
 * ============================================================================ */

/**
 * @brief Execute CPUID instruction
 * 
 * @param leaf  Function number (EAX input)
 * @param eax   Output EAX
 * @param ebx   Output EBX
 * @param ecx   Output ECX
 * @param edx   Output EDX
 */
static ALWAYS_INLINE void cpuid(uint32_t leaf, 
                                uint32_t *eax, uint32_t *ebx,
                                uint32_t *ecx, uint32_t *edx) {
    __asm__ volatile(
        "cpuid"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(leaf), "c"(0)
    );
}

#endif /* _ARCH_X86_64_H */
