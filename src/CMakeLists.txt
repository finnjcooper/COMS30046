cmake_minimum_required(VERSION 3.14)

project(main CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(ftxui
	GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
	GIT_TAG v6.1.9
)

FetchContent_Declare(argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse
	GIT_TAG v3.2
)

FetchContent_Declare(nlohmann_json
	GIT_REPOSITORY https://github.com/nlohmann/json
	GIT_TAG v3.11.3
)

FetchContent_Declare(elfio
	GIT_REPOSITORY https://github.com/serge1/ELFIO
	GIT_TAG Release_3.12
)

FetchContent_MakeAvailable(argparse elfio ftxui nlohmann_json)


file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "*.cpp")
add_executable(main ${SOURCES})

add_library(lib INTERFACE)
target_include_directories(lib INTERFACE lib/)
target_include_directories(main PRIVATE cpu utils tui)

target_link_libraries(main PRIVATE
	lib
	ftxui::screen
	ftxui::dom
	ftxui::component
	argparse::argparse
	elfio::elfio
	nlohmann_json::nlohmann_json
)

set(RISCV_GCC "riscv64-unknown-elf-gcc")
set(RISCV_OBJDUMP "riscv64-unknown-elf-objdump")

set(RISCV_CFLAGS
	-march=rv32im
	-mabi=ilp32
	-mno-relax
	-O2
	-ffreestanding
	-fno-pic
	-fno-pie
	-fno-builtin
	-fno-stack-protector
)

set(LINKER ${CMAKE_SOURCE_DIR}/test/startup/linker.ld)
set(RISCV_LDFLAGS
	-nostdlib
	-static
	-no-pie
	-T ${LINKER}
)

set(BENCH_SOURCE_DIR ${CMAKE_SOURCE_DIR}/bench/src)
set(BENCH_BINARY_DIR ${CMAKE_SOURCE_DIR}/bench/build)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/test/*.c")
file(GLOB BENCH_SOURCES CONFIGURE_DEPENDS "${BENCH_SOURCE_DIR}/*.c")

foreach(TEST_SOURCE ${TEST_SOURCES})
	get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
	
	set(ELF_OUTPUT ${CMAKE_BINARY_DIR}/${TEST_NAME}.elf)
	set(ASM_OUTPUT ${CMAKE_BINARY_DIR}/${TEST_NAME}.asm)
	
	add_custom_command(
		OUTPUT ${ELF_OUTPUT}
		COMMAND ${RISCV_GCC} ${RISCV_CFLAGS} ${RISCV_LDFLAGS} ${TEST_SOURCE} -o ${ELF_OUTPUT}
		COMMAND ${RISCV_OBJDUMP} -d ${ELF_OUTPUT} > ${ASM_OUTPUT}
		DEPENDS ${TEST_SOURCE} ${LINKER}
		COMMENT "Building RISC-V test: ${TEST_NAME}"
	)
	
	add_custom_target(${TEST_NAME}_elf ALL DEPENDS ${ELF_OUTPUT})
endforeach()

foreach(BENCH_SOURCE ${BENCH_SOURCES})
	get_filename_component(BENCH_NAME ${BENCH_SOURCE} NAME_WE)
	
	set(ELF_OUTPUT ${BENCH_BINARY_DIR}/${BENCH_NAME}.elf)
	set(ASM_OUTPUT ${BENCH_BINARY_DIR}/${BENCH_NAME}.asm)
	
	add_custom_command(
		OUTPUT ${ELF_OUTPUT}
		COMMAND ${RISCV_GCC} ${RISCV_CFLAGS} ${RISCV_LDFLAGS} ${BENCH_SOURCE} -o ${ELF_OUTPUT}
		COMMAND ${RISCV_OBJDUMP} -d ${ELF_OUTPUT} > ${ASM_OUTPUT}
		DEPENDS ${BENCH_SOURCE} ${LINKER}
		COMMENT "Building RISC-V benchmark: ${BENCH_NAME}"
	)
	
	add_custom_target(${BENCH_NAME}_bench_elf ALL DEPENDS ${ELF_OUTPUT})
endforeach()
