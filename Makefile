# ==============================================================================
# Squirel OS - Build System (Linux/WSL)
# ==============================================================================
# A minimal x86_64 operating system
#
# Targets:
#   all       - Build bootloader, kernel, and disk image
#   bootloader - Build bootloader only
#   kernel    - Build kernel only
#   image     - Create bootable disk image
#   run       - Run in QEMU
#   debug     - Run in QEMU with GDB server
#   clean     - Remove build artifacts
# ==============================================================================

# Toolchain Configuration
CC      := gcc
LD      := ld
ASM     := nasm
OBJCOPY := objcopy

# Directories
BUILD_DIR   := build
BOOT_DIR    := boot
KERNEL_DIR  := kernel
INCLUDE_DIR := include

# Output files
BOOTLOADER_BIN := $(BUILD_DIR)/bootloader.bin
KERNEL_BIN     := $(BUILD_DIR)/kernel.bin
KERNEL_ELF     := $(BUILD_DIR)/kernel.elf
DISK_IMAGE     := $(BUILD_DIR)/squirel.img

# ==============================================================================
# Compiler Flags
# ==============================================================================
CFLAGS := -m64 \
          -ffreestanding \
          -nostdlib \
          -fno-builtin \
          -fno-stack-protector \
          -mno-red-zone \
          -mcmodel=large \
          -fno-pic \
          -fno-pie \
          -Wall -Wextra \
          -O2 \
          -I$(INCLUDE_DIR) \
          -I$(KERNEL_DIR)

# Linker flags
LDFLAGS := -nostdlib -static -z max-page-size=0x1000

# Assembly flags
ASM_BIN  := -f bin
ASM_ELF  := -f elf64

# ==============================================================================
# Source Files
# ==============================================================================

# Object files
KERNEL_OBJ := $(BUILD_DIR)/start64.o \
              $(BUILD_DIR)/interrupts.o \
              $(BUILD_DIR)/kmain.o \
              $(BUILD_DIR)/gdt.o \
              $(BUILD_DIR)/idt.o \
              $(BUILD_DIR)/port.o \
              $(BUILD_DIR)/vga_text.o \
              $(BUILD_DIR)/keyboard.o \
              $(BUILD_DIR)/serial.o \
              $(BUILD_DIR)/string.o \
              $(BUILD_DIR)/memory.o \
              $(BUILD_DIR)/printf.o \
              $(BUILD_DIR)/shell.o \
              $(BUILD_DIR)/parser.o \
              $(BUILD_DIR)/cmd_help.o \
              $(BUILD_DIR)/cmd_clear.o \
              $(BUILD_DIR)/cmd_echo.o \
              $(BUILD_DIR)/cmd_info.o

# ==============================================================================
# Main Targets
# ==============================================================================

.PHONY: all bootloader kernel image run debug clean

all: image
	@echo "========================================"
	@echo "  Squirel OS build complete!"
	@echo "  Run with: make run"
	@echo "========================================"

# ==============================================================================
# Directory Creation
# ==============================================================================

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ==============================================================================
# Bootloader
# ==============================================================================

bootloader: $(BOOTLOADER_BIN)

$(BUILD_DIR)/mbr.bin: $(BOOT_DIR)/stage1/mbr.asm | $(BUILD_DIR)
	@echo "[ASM] mbr.asm"
	$(ASM) $(ASM_BIN) $< -o $@

$(BUILD_DIR)/stage2.bin: $(BOOT_DIR)/stage2/loader.asm | $(BUILD_DIR)
	@echo "[ASM] loader.asm"
	$(ASM) $(ASM_BIN) $< -o $@

$(BOOTLOADER_BIN): $(BUILD_DIR)/mbr.bin $(BUILD_DIR)/stage2.bin
	@echo "[CAT] Creating bootloader..."
	cat $(BUILD_DIR)/mbr.bin $(BUILD_DIR)/stage2.bin > $(BOOTLOADER_BIN)

# ==============================================================================
# Kernel Assembly
# ==============================================================================

$(BUILD_DIR)/start64.o: $(KERNEL_DIR)/entry/start64.asm | $(BUILD_DIR)
	@echo "[ASM] start64.asm"
	$(ASM) $(ASM_ELF) $< -o $@

$(BUILD_DIR)/interrupts.o: $(KERNEL_DIR)/arch/x86_64/cpu/interrupts.asm | $(BUILD_DIR)
	@echo "[ASM] interrupts.asm"
	$(ASM) $(ASM_ELF) $< -o $@

# ==============================================================================
# Kernel C Files
# ==============================================================================

$(BUILD_DIR)/kmain.o: $(KERNEL_DIR)/entry/kmain.c | $(BUILD_DIR)
	@echo "[CC] kmain.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/gdt.o: $(KERNEL_DIR)/arch/x86_64/cpu/gdt.c | $(BUILD_DIR)
	@echo "[CC] gdt.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/idt.o: $(KERNEL_DIR)/arch/x86_64/cpu/idt.c | $(BUILD_DIR)
	@echo "[CC] idt.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/port.o: $(KERNEL_DIR)/arch/x86_64/io/port.c | $(BUILD_DIR)
	@echo "[CC] port.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/vga_text.o: $(KERNEL_DIR)/drivers/vga/vga_text.c | $(BUILD_DIR)
	@echo "[CC] vga_text.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/keyboard.o: $(KERNEL_DIR)/drivers/keyboard/keyboard.c | $(BUILD_DIR)
	@echo "[CC] keyboard.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/serial.o: $(KERNEL_DIR)/drivers/serial/serial.c | $(BUILD_DIR)
	@echo "[CC] serial.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/string.o: $(KERNEL_DIR)/lib/string/string.c | $(BUILD_DIR)
	@echo "[CC] string.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/memory.o: $(KERNEL_DIR)/lib/memory/memory.c | $(BUILD_DIR)
	@echo "[CC] memory.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/printf.o: $(KERNEL_DIR)/lib/printf/printf.c | $(BUILD_DIR)
	@echo "[CC] printf.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/shell.o: $(KERNEL_DIR)/shell/shell.c | $(BUILD_DIR)
	@echo "[CC] shell.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/parser.o: $(KERNEL_DIR)/shell/parser/parser.c | $(BUILD_DIR)
	@echo "[CC] parser.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/cmd_help.o: $(KERNEL_DIR)/shell/commands/cmd_help.c | $(BUILD_DIR)
	@echo "[CC] cmd_help.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/cmd_clear.o: $(KERNEL_DIR)/shell/commands/cmd_clear.c | $(BUILD_DIR)
	@echo "[CC] cmd_clear.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/cmd_echo.o: $(KERNEL_DIR)/shell/commands/cmd_echo.c | $(BUILD_DIR)
	@echo "[CC] cmd_echo.c"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/cmd_info.o: $(KERNEL_DIR)/shell/commands/cmd_info.c | $(BUILD_DIR)
	@echo "[CC] cmd_info.c"
	$(CC) $(CFLAGS) -c $< -o $@

# ==============================================================================
# Kernel Linking
# ==============================================================================

kernel: $(KERNEL_BIN)

$(KERNEL_ELF): $(KERNEL_OBJ) | $(BUILD_DIR)
	@echo "[LD] Linking kernel..."
	$(LD) $(LDFLAGS) -T link/kernel.ld -o $@ $(KERNEL_OBJ)

$(KERNEL_BIN): $(KERNEL_ELF)
	@echo "[OBJCOPY] Creating kernel binary..."
	$(OBJCOPY) -O binary $< $@

# ==============================================================================
# Disk Image
# ==============================================================================

image: bootloader kernel
	@echo "[IMAGE] Creating disk image..."
	dd if=/dev/zero of=$(DISK_IMAGE) bs=512 count=2880 2>/dev/null
	dd if=$(BOOTLOADER_BIN) of=$(DISK_IMAGE) conv=notrunc 2>/dev/null
	dd if=$(KERNEL_BIN) of=$(DISK_IMAGE) bs=512 seek=17 conv=notrunc 2>/dev/null
	@echo "[DONE] $(DISK_IMAGE) created successfully!"

# ==============================================================================
# Run in QEMU
# ==============================================================================

run: image
	@echo "[QEMU] Starting Squirel OS..."
	qemu-system-x86_64 -drive format=raw,file=$(DISK_IMAGE) -serial stdio -m 128M

debug: image
	@echo "[QEMU] Starting in debug mode (GDB on port 1234)..."
	qemu-system-x86_64 -drive format=raw,file=$(DISK_IMAGE) -serial stdio -m 128M -s -S

# ==============================================================================
# Clean
# ==============================================================================

clean:
	@echo "[CLEAN] Removing build artifacts..."
	rm -rf $(BUILD_DIR)
	@echo "[DONE] Clean complete"
