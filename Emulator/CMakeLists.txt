cmake_minimum_required(VERSION 3.24)

project(rose_emu VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(SDLite REQUIRED)

add_executable(rose_emu
    src/core/test.c 
    src/core/init.c
    src/core/iterate.c
    src/core/destroy.c 
    src/hardware/emulator.c 
    src/hardware/alu.c 
    src/hardware/fpu.c
    src/pipeline/mmio.c
    src/pipeline/fetch.c
    src/pipeline/execute.c
    src/pipeline/decode.c 
    src/pipeline/execute/execute_memory.c 
    src/pipeline/execute/execute_math.c
    src/main.c
)

target_include_directories(rose_emu PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)


target_compile_definitions(rose_emu
    PRIVATE
        TRACK_ALLOCATIONS
)

target_link_libraries(rose_emu
    PUBLIC
        SDLite::debugSDLite
    PRIVATE
        m
)