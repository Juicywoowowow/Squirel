/**
 * @file kmain.c
 * @brief Kernel main entry point
 * 
 * This is where the kernel starts after the bootloader has set up
 * 64-bit long mode and jumped to our code. We initialize all
 * subsystems and start the interactive shell.
 * 
 * INITIALIZATION ORDER:
 *   1. VGA driver (so we can display output)
 *   2. Serial port (for QEMU debug output)
 *   3. Keyboard driver (for user input)
 *   4. Shell (main user interface)
 * 
 * @note This function should NEVER return. If it does, the CPU halts.
 */

#include <squirel/types.h>
#include <squirel/config.h>
#include <drivers/vga/vga_text.h>
#include <drivers/keyboard/keyboard.h>
#include <drivers/serial/serial.h>
#include <lib/printf/printf.h>
#include <shell/shell.h>

/**
 * @brief Kernel main function
 * 
 * Called by start64.asm after long mode setup.
 * This function never returns.
 */
NORETURN void kmain(void) {
    /* ====================================================================
     * Phase 1: Early Initialization
     * ==================================================================== */
    
    /* Initialize VGA text mode - must be first for visual output */
    vga_init();
    
    /* Print early boot message */
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_print("[OK] ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_println("VGA text mode initialized");
    
    /* Initialize serial port for debug output */
    serial_init();
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_print("[OK] ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_println("Serial port initialized (COM1)");
    
    /* Send message to serial for QEMU console */
    serial_print("Squirel OS booting...\n");
    
    /* ====================================================================
     * Phase 2: Hardware Initialization
     * ==================================================================== */
    
    /* Initialize keyboard */
    keyboard_init();
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_print("[OK] ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_println("Keyboard initialized");
    
    /* ====================================================================
     * Phase 3: Start Shell
     * ==================================================================== */
    
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    vga_print("[OK] ");
    vga_set_color(VGA_WHITE, VGA_BLACK);
    vga_println("Starting shell...");
    
    /* Run the shell - this never returns */
    shell_run();
    
    /* Should never reach here, but just in case... */
    vga_set_color(VGA_RED, VGA_BLACK);
    vga_println("\n!!! Kernel panic: shell_run() returned !!!");
    
    for (;;) {
        __asm__ volatile("cli; hlt");
    }
}
