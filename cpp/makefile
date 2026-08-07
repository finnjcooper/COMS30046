.PHONY: clean wasm native

clean:
	cmake -E remove_directory build

native:
	cmake -S src -B build/native -G Ninja
	cmake --build build/native -j

wasm:
	emcmake cmake -S src -B build/wasm -G Ninja
	cmake --build build/wasm -j
