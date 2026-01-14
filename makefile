RISCV_GCC = riscv64-unknown-elf-gcc
CXX = g++

CFLAGS = \
	-march=rv32i \
	-mabi=ilp32 -mno-relax \
	-O0 \
	-ffreestanding \
	-fno-pic \
	-fno-builtin \
	-fno-stack-protector \
	-nostdlib -nostartfiles \
	-T test/linker.ld

CXXFLAGS = -I src -std=c++26 -g


CSRCS := test/test.c
CASMS := test/start.s $(patsubst test/%.c, test/%.s, $(CSRCS))

CXXSRCS := src/main.cpp
CXXOBJS := $(patsubst src/%.cpp, build/%.o, $(CXXSRCS))

default: run

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $^ -o $@

test/%.s: test/%.c
	$(RISCV_GCC) $(CFLAGS) -S $^ -o $@

cpu: $(CXXOBJS)
	$(CXX) $^ -o build/$@.exe

bench: $(CASMS) | build
	$(RISCV_GCC) $(CFLAGS) $^ -o build/$@.elf
	riscv64-unknown-elf-objdump -d build/bench.elf > build/bench.asm
# 	riscv64-unknown-elf-objcopy -O binary --only-section=.text build/$@.elf build/$@.bin


run: cpu | bench
	build/cpu.exe build/bench.elf

build:
	mkdir build

clean:
ifeq ($(OS), Windows_NT)
	rmdir /s /q build
else
	rm -rf build
endif