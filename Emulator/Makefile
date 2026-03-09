
GCC = gcc

SRC = $(wildcard src/*.c src/emulator/*.c src/pipeline/*.c)
OUT = build/CEMU.exe

INCLUDE = -ISDLite/include -Iinclude  
LIB = -LSDLite/lib 
LINK = -lSDL3 -lSDL3_image -lSDL3_mixer -lSDL3_ttf


W_FLAGS = -Wall -Werror -Wpedantic
OPT_FLAG = -O3 -s -DSTDLIB_ALLOCATIONS
DEBUG_FLAG = -g -DNDEBUG -DTRACK_ALLOCATIONS


default: debug_build run


opt: opt_build run


debug_build:
	@${GCC} -o ${OUT} ${SRC} -lSDLite_debug ${LINK} ${LIB} ${INCLUDE} ${W_FLAGS} ${DEBUG_FLAG}


opt_build:
	@${GCC} -o ${OUT} ${SRC} -lSDLite ${LINK} ${LIB} ${INCLUDE} ${W_FLAGS} ${OPT_FLAG}


run:
	@${OUT}





