/**
 * @file cmd_info.c
 * @brief Info command implementation
 * 
 * Displays system information including OS version and basic stats.
 */

#include <shell/shell.h>
#include <squirel/config.h>
#include <lib/printf/printf.h>
#include <drivers/vga/vga_text.h>

/**
 * @brief Info command handler
 * 
 * Usage:
 *   info
 * 
 * Displays:
 *   - OS name and version
 *   - Architecture
 *   - Build information
 */
void cmd_info(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    kprintf("\n");
    
    vga_set_color(VGA_YELLOW, VGA_BLACK);
    kprintf("System Information\n");
    vga_set_color(VGA_LIGHT_GRAY, VGA_BLACK);
    kprintf("==================\n\n");
    
    kprintf("  OS Name:      %s\n", SQUIREL_NAME);
    kprintf("  Version:      %s\n", SQUIREL_VERSION_STRING);
    kprintf("  Architecture: x86_64 (64-bit)\n");
    kprintf("  Video Mode:   VGA Text (80x25, 16 colors)\n");
    kprintf("\n");
    
    kprintf("Memory:\n");
    kprintf("  Kernel Load:  0x%X\n", KERNEL_LOAD_ADDR);
    kprintf("  Stack Top:    0x%X\n", KERNEL_STACK_TOP);
    kprintf("\n");
}
