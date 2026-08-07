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
	core/utils/simulator.cpp
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
	set_target_properties(simnative PROPERTIES OUTPUT_NAME "sim")
else ()
	add_executable(simwasm wasm/bindings.cpp)
	target_link_libraries(simwasm PRIVATE simcore)
	target_link_options(simwasm PRIVATE
		"-lembind"
		"--emit-tsd"
		"${CMAKE_CURRENT_SOURCE_DIR}/../../web/src/wasm/sim.d.ts"
		"-sMODULARIZE=1"
		"-sEXPORT_ES6=1"
		"-sENVIRONMENT=web"
		"-sALLOW_MEMORY_GROWTH=1"
		"-sFILESYSTEM=0"
		"-sEXPORT_NAME=createModule"
		"-o${CMAKE_CURRENT_SOURCE_DIR}/../../web/src/wasm/sim.mjs"
	)
	set_target_properties(simwasm PROPERTIES OUTPUT_NAME "sim")
endif ()
