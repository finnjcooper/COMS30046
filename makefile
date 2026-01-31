
BENCH ?= add
PIPELINED ?= true
FORWARDING ?= true

PIPELINED_FLAG :=
ifeq ($(PIPELINED),true)
	PIPELINED_FLAG := -p
endif

FORWARDING_FLAG :=
ifeq ($(FORWARDING),true)
	FORWARDING_FLAG := -f
endif

default: build
	.\build\main.exe --elf .\build\$(BENCH).elf $(PIPELINED_FLAG) $(FORWARDING_FLAG)

build:
	cmake --preset default src && ninja -C build

clean:
ifeq ($(OS),Windows_NT)
	rmdir /s /q build
else
	rm -rf build
endif