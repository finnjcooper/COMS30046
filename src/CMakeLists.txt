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

FetchContent_MakeAvailable(ftxui)


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
)

set(RISCV_GCC "riscv64-unknown-elf-gcc")
set(RISCV_OBJDUMP "riscv64-unknown-elf-objdump")

set(RISCV_CFLAGS
	-march=rv32im
	-mabi=ilp32
	-mno-relax
	-O0
	-ffreestanding
	-fno-pic
	-fno-pie
	-fno-builtin
	-fno-stack-protector
)

set(RISCV_LDFLAGS
	-nostdlib
	-static
	-no-pie
	-T ${CMAKE_SOURCE_DIR}/test/startup/linker.ld
)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "test/*.c")

foreach(TEST_SOURCE ${TEST_SOURCES})
	get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
	
	set(ELF_OUTPUT ${CMAKE_BINARY_DIR}/${TEST_NAME}.elf)
	set(ASM_OUTPUT ${CMAKE_BINARY_DIR}/${TEST_NAME}.asm)
	
	add_custom_command(
		OUTPUT ${ELF_OUTPUT}
		COMMAND ${RISCV_GCC} ${RISCV_CFLAGS} ${RISCV_LDFLAGS}
			${TEST_SOURCE} ${CMAKE_SOURCE_DIR}/test/startup/start.s
			-o ${ELF_OUTPUT}
		COMMAND ${RISCV_OBJDUMP} -d ${ELF_OUTPUT} > ${ASM_OUTPUT}
		DEPENDS ${TEST_SOURCE} ${CMAKE_SOURCE_DIR}/test/startup/start.s ${CMAKE_SOURCE_DIR}/test/startup/linker.ld
		COMMENT "Building RISC-V benchmark: ${TEST_NAME}"
	)
	
	add_custom_target(${TEST_NAME}_elf ALL DEPENDS ${ELF_OUTPUT})
endforeach()
