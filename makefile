
BENCH ?= add
PIPELINED ?= true
FORWARDING ?= true

PIPELINED_FLAG :=
ifeq ($(PIPELINED),true)
	PIPELINED_FLAG := --pipelined
endif

FORWARDING_FLAG :=
ifeq ($(FORWARDING),true)
	FORWARDING_FLAG := --forwarding
endif

default: ninja
	.\build\main.exe --elf .\build\$(BENCH).elf $(PIPELINED_FLAG) $(FORWARDING_FLAG)

ninja: src/
	ninja -C build

cmake:
	cmake --preset default src

clean:
ifeq ($(OS),Windows_NT)
	rmdir /s /q build
else
	rm -rf build
endif