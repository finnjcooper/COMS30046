RISCV_GCC = riscv64-unknown-elf-gcc
RISCV_OBJDUMP = riscv64-unknown-elf-objdump
CXX = g++

CFLAGS = \
	-march=rv32im \
	-mabi=ilp32 -mno-relax \
	-O0 \
	-ffreestanding \
	-fno-pic \
	-fno-builtin \
	-fno-stack-protector \
	-nostdlib -nostartfiles

CLDFLAGS = -T test/linker.ld

CXXFLAGS = -I src -std=c++26 -g -DNCURSES_STATIC
CXXLDFLAGS = -lncurses

ifeq ($(OS), Windows_NT)
	CXXFLAGS += -I G:\Finn\Stuff\MSYS2\mingw64\include\ncurses
endif

BENCH ?= add


CSRCS := $(wildcard test/*.c)
CXXSRCS := $(wildcard src/*.cpp)
CXXOBJS := $(patsubst src/%.cpp,build/%.o,$(CXXSRCS))
DEPS := $(CXXOBJS:.o=.d)

-include $(DEPS)

.PHONY: run clean
.DEFAULT_GOAL := run

build/%.elf: test/%.c test/start.s | build
	$(RISCV_GCC) $(CFLAGS) $(CLDFLAGS) $^ -o $@
	$(RISCV_OBJDUMP) -d $@ > $(@:.elf=.asm)
# 	riscv64-unknown-elf-objcopy -O binary --only-section=.text build/$@.elf build/$@.bin

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

build/main.exe: $(CXXOBJS) | build
	$(CXX) $^ $(CXXLDFLAGS) -o $@

run: build/main.exe build/$(BENCH).elf
	$^

build:
	mkdir build

clean:
ifeq ($(OS), Windows_NT)
	rmdir /s /q build
else
	rm -rf build
endif