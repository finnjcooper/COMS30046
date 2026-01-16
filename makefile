RISCV_GCC = riscv64-unknown-elf-gcc
RISCV_OBJDUMP = riscv64-unknown-elf-objdump
CXX = g++

CFLAGS = \
	-march=rv32i \
	-mabi=ilp32 -mno-relax \
	-O0 \
	-ffreestanding \
	-fno-pic \
	-fno-builtin \
	-fno-stack-protector \
	-nostdlib -nostartfiles

CLDFLAGS = -T test/linker.ld

CXXFLAGS = -I src -std=c++26 -g

ifeq ($(OS), Windows_NT)
	CXXLDFLAGS = -lpdcurses
else
	CXXLDFLAGS = -lncurses
endif

BENCH ?= add


CSRCS := $(wildcard test/*.c)
CXXSRCS := $(wildcard src/*.cpp)

default: run

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.elf: test/%.c test/start.s | build
	$(RISCV_GCC) $(CFLAGS) $(CLDFLAGS) $^ -o $@
	$(RISCV_OBJDUMP) -d $@ > $(@:.elf=.asm)

main: $(CXXSRCS:src/%.cpp=build/%.o)
	$(CXX) $^ $(CXXLDFLAGS) -o build/$@.exe

# 	riscv64-unknown-elf-objcopy -O binary --only-section=.text build/$@.elf build/$@.bin


run: main build/$(BENCH).elf
	build/$<.exe build/$(BENCH).elf

build:
	mkdir build

clean:
ifeq ($(OS), Windows_NT)
	rmdir /s /q build
else
	rm -rf build
endif