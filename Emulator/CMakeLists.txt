cmake_minimum_required(VERSION 3.24)

project(rose_emu VERSION 1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(SDLite REQUIRED)

add_executable(rose_emu
    src/init.c 
    src/iterate.c 
    src/render.c 
    src/quit.c 
    src/update.c 
    src/sdl_events.c 
    src/frame_events.c 
)

target_include_directories(rose_emu PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)


target_compile_definitions(rose_emu
    PRIVATE
        TRACK_ALLOCATIONS
)

target_link_libraries(rose_emu 
    SDLite::debugSDLite
)