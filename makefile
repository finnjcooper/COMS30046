KERNEL ?= vectoradd
HEADLESS ?= 0

HEADLESS_FLAG := $(if $(filter 1,$(HEADLESS)),--headless,)

ifeq ($(OS),Windows_NT)
	HAVE_NINJA := $(strip $(shell where.exe ninja 2>NUL))
else
	HAVE_NINJA := $(strip $(shell command -v ninja 2>/dev/null))
endif

.PHONY: all cmake clean

default: test

headless:
	$(MAKE) HEADLESS=1


bench: all
ifeq ($(OS),Windows_NT)
	.\build\main.exe --elf .\src\bench\build\$(KERNEL).elf $(HEADLESS_FLAG)
else
	./build/main --elf ./src/bench/build/$(KERNEL).elf $(HEADLESS_FLAG)
endif

test: all
ifeq ($(OS),Windows_NT)
	.\build\main.exe --elf .\build\$(KERNEL).elf $(HEADLESS_FLAG)
else
	./build/main --elf ./build/$(KERNEL).elf $(HEADLESS_FLAG)
endif

all: src/
	cmake --build build -j


cmake:
ifneq ($(HAVE_NINJA),)
	@echo Using Ninja.
	@cmake --preset ninja src
else
	@echo Using Make.
	@cmake --preset make src
endif

clean:
ifeq ($(OS),Windows_NT)
	rmdir /s /q build
else
	rm -rf build
endif
