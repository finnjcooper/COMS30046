.DEFAULT_GOAL := all
.PHONY: all clean bench benchclean wasm native

all: native wasm
	cmake --build build/native -j
	cmake --build build/wasm -j

clean:
	cmake -E remove_directory build

bench:
	make -C bench

benchclean:
	make -C bench clean

native:
	cmake -S src -B build/native -G Ninja

wasm:
	emcmake cmake -S src -B build/wasm -G Ninja
