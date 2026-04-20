RISCV_GCC := riscv64-unknown-elf-gcc
RISCV_OBJDUMP := riscv64-unknown-elf-objdump

SRC_DIR := src
BUILD_DIR := build
STARTUP := $(SRC_DIR)/entry/start.s
LINKER := $(SRC_DIR)/entry/linker.ld

SOURCES := $(wildcard $(SRC_DIR)/*.c)
ELFS := $(patsubst ${SRC_DIR}/%.c,${BUILD_DIR}/%.elf,$(SOURCES))

RISCV_CFLAGS := \
	-march=rv32imfv \
	-mabi=ilp32f \
	-mno-relax \
	-O2 \
	-ffreestanding \
	-fno-pic \
	-fno-pie \
	-fno-builtin \
	-fno-stack-protector

RISCV_LDFLAGS := \
	-nostdlib \
	-static \
	-no-pie \
	-T $(LINKER)

.DEFAULT_GOAL := all

.PHONY: all clean

all: $(ELFS)

$(BUILD_DIR):
	cmake -E make_directory $@

$(BUILD_DIR)/%.elf: $(SRC_DIR)/%.c $(STARTUP) $(LINKER) | $(BUILD_DIR)
	$(RISCV_GCC) $(RISCV_CFLAGS) $(RISCV_LDFLAGS) $(STARTUP) $< -o $@

clean:
	cmake -E rm -f $(ELFS)
