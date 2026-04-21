BENCH ?= 
CONFIG ?= 

BENCH_FILE := 
ifneq ($(BENCH),)
	BENCH_FILE := bench/build/$(BENCH).elf
endif

CONFIG_FLAG := 
ifneq ($(CONFIG),)
	CONFIG_FLAG := --config $(CONFIG)
endif

ifeq ($(OS),Windows_NT)
	HAVE_NINJA := $(strip $(shell where.exe ninja 2>NUL))
else
	HAVE_NINJA := $(strip $(shell command -v ninja 2>/dev/null))
endif

.DEFAULT_GOAL := all
.PHONY: all clean bench benchclean run headless

all: build/CMakeCache.txt
	cmake --build build -j

clean:
	cmake -E remove_directory build

bench:
	make -C bench

benchclean:
	make -C bench clean

run: all
	build/main.exe $(BENCH_FILE) $(CONFIG_FLAG)

headless: all
	build/main.exe $(BENCH_FILE) $(CONFIG_FLAG) --headless


build/CMakeCache.txt: src/CMakeLists.txt
ifneq ($(HAVE_NINJA),)
	cmake --preset ninja src
else
	cmake --preset make src
endif
