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

file(GLOB_RECURSE MAIN_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(main ${MAIN_SOURCES})
set_target_properties(main PROPERTIES OUTPUT_NAME main SUFFIX ".exe")

target_include_directories(main PRIVATE cpu utils tui)

target_link_libraries(main PRIVATE
	ftxui::screen
	ftxui::dom
	ftxui::component
	argparse::argparse
	elfio::elfio
	nlohmann_json::nlohmann_json
)
