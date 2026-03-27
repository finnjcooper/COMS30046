BENCH ?= add
HEADLESS ?= 0

HEADLESS_FLAG := $(if $(filter 1,$(HEADLESS)),--headless,)

default: ninja
ifeq ($(OS),Windows_NT)
	.\build\main.exe --elf .\build\$(BENCH).elf $(HEADLESS_FLAG)
else
	./build/main --elf ./build/$(BENCH).elf $(HEADLESS_FLAG)
endif

headless:
	$(MAKE) HEADLESS=1


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
