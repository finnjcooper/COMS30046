cmake_minimum_required(VERSION 3.14)

project(sim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(nlohmann_json
	GIT_REPOSITORY https://github.com/nlohmann/json
	GIT_TAG v3.11.3
)

FetchContent_Declare(elfio
	GIT_REPOSITORY https://github.com/serge1/ELFIO
	GIT_TAG Release_3.12
)

FetchContent_MakeAvailable(elfio nlohmann_json)

add_library(simcore
	core/cpu/cpu.cpp
	core/utils/loader.cpp
)

target_include_directories(simcore PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/core/cpu
	${CMAKE_CURRENT_SOURCE_DIR}/core/utils
)

target_link_libraries(simcore PUBLIC
	elfio::elfio
	nlohmann_json::nlohmann_json
)

if (NOT EMSCRIPTEN)
	add_executable(simnative native/main.cpp)
	target_link_libraries(simnative PRIVATE simcore)
else ()
	add_executable(simwasm wasm/bindings.cpp wasm/simulator.cpp)
	target_link_libraries(simwasm PRIVATE simcore)
	target_link_options(simwasm PRIVATE
		"-lembind"

		"SHELL:-sMODULARIZE=1"
		"SHELL:-sEXPORT_ES6=1"
		"SHELL:-sENVIRONMENT=web"
		"SHELL:-sALLOW_MEMORY_GROWTH=1"
		"SHELL:-sFILESYSTEM=0"
		"SHELL:-sEXPORT_NAME=createModule"
	)
	set_target_properties(simwasm PROPERTIES
		OUTPUT_NAME "sim"
		SUFFIX ".mjs"
	)
endif ()
