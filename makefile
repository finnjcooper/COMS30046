
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
ifeq ($(OS),Windows_NT)
	.\build\main.exe --elf .\src\test\build\$(BENCH).elf $(PIPELINED_FLAG) $(FORWARDING_FLAG)
else
	./build/main --elf ./src/test/build/$(BENCH).elf $(PIPELINED_FLAG) $(FORWARDING_FLAG)
endif

ninja: src/
	ninja -C build

make: src/
	make -C build

cmake:
	cmake --preset default src

clean:
ifeq ($(OS),Windows_NT)
	rmdir /s /q build
else
	rm -rf build
endif
