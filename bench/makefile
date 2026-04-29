RISCV_GCC := riscv64-unknown-elf-gcc
RISCV_OBJDUMP := riscv64-unknown-elf-objdump

SRC_DIR := src
BUILD_DIR := build
LINKER := $(SRC_DIR)/lib/linker.ld

SOURCES := $(wildcard $(SRC_DIR)/*.c)
ELFS := $(patsubst ${SRC_DIR}/%.c,${BUILD_DIR}/%.elf,$(SOURCES))
ASMS := $(patsubst ${BUILD_DIR}/%.elf,${BUILD_DIR}/%.asm,$(ELFS))

RISCV_BASE_CFLAGS := \
	-mabi=ilp32f \
	-mno-relax \
	-O2 \
	-ffreestanding \
	-fno-pic \
	-fno-pie \
	-fno-builtin \
	-fno-stack-protector

RISCV_SCALAR_CFLAGS := \
	-march=rv32imf \
	$(RISCV_BASE_CFLAGS)

RISCV_VECTOR_CFLAGS := \
	-march=rv32imfv \
	$(RISCV_BASE_CFLAGS)

RISCV_LDFLAGS := \
	-nostdlib \
	-static \
	-no-pie \
	-T $(LINKER)

.DEFAULT_GOAL := all

.PHONY: all clean

all: $(ELFS) $(ASMS)

$(BUILD_DIR):
	cmake -E make_directory $@

$(BUILD_DIR)/%v.elf: $(SRC_DIR)/%v.c $(LINKER) | $(BUILD_DIR)
	$(RISCV_GCC) $(RISCV_VECTOR_CFLAGS) $(RISCV_LDFLAGS) $< -o $@

$(BUILD_DIR)/%.elf: $(SRC_DIR)/%.c $(LINKER) | $(BUILD_DIR)
	$(RISCV_GCC) $(RISCV_SCALAR_CFLAGS) $(RISCV_LDFLAGS) $< -o $@

$(BUILD_DIR)/%.asm: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(RISCV_OBJDUMP) -d $< > $@

clean:
	cmake -E rm -f $(ELFS) $(ASMS)
