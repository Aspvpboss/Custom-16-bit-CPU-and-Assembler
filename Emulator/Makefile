
GCC = gcc

SRC = $(wildcard src/*.c src/emulator/*.c src/pipeline/*.c src/pipeline/execute/*.c src/core/*.c src/hardware/*.c)
OUT = build/CEMU.exe

INCLUDE = -Isrc
LIB = 
LINK = -lSDL3 -lSDL3_image -lSDL3_mixer -lSDL3_ttf


W_FLAGS = -Wall -Werror -Wpedantic
OPT_FLAG = -O2 -s -DNDEBUG -DSTDLIB_ALLOCATIONS
DEBUG_FLAG = -g -DTRACK_ALLOCATIONS


default: debug_build run


opt: opt_build run


debug_build:
	@${GCC} -o ${OUT} ${SRC} -lSDLite_debug ${LINK} ${LIB} ${INCLUDE} ${W_FLAGS} ${DEBUG_FLAG}


opt_build:
	@${GCC} -o ${OUT} ${SRC} -lSDLite ${LINK} ${LIB} ${INCLUDE} ${W_FLAGS} ${OPT_FLAG}


run:
	@${OUT}





