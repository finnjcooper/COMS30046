RISCV_GCC = riscv64-unknown-elf-gcc
CXX = g++

CFLAGS = \
  -march=rv32i \
  -mabi=ilp32 \
  -O0 \
  -ffreestanding \
  -fno-builtin \
  -fno-stack-protector \
  -nostdlib

CXXFLAGS = -I src -std=c++26 -g


SRCS := src/cpu.cpp src/alu.cpp src/memory.cpp src/registers.cpp src/program.cpp
OBJS := $(patsubst src/%.cpp, build/%.o, $(SRCS))


default: cpu

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

cpu: $(OBJS)
	$(CXX) $(OBJS) -o build/$@.exe

build:
	mkdir -p build


run: cpu
	build/cpu.exe

test: test/test.c
	$(RISCV_GCC) -S $< -o test/$@.s $(CFLAGS)

clean:
	rm -rf build