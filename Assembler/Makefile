CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
OUT = assembler.exe

BUILD_DIR = build
SRC = ${wildcard src/*.c}

INCLUDE = -Isrc
OBJ = ${SRC:.c=.o}

.PHONY: debug_build run clean
default: debug_build run



debug_build: ${OBJ}
	${CC} ${CFLAGS} ${INCLUDE} -DTRACK_ALLOCATIONS -g -o ${BUILD_DIR}/${OUT} ${OBJ}

opt_build: ${OBJ}
	${CC} ${CFLAGS} ${INCLUDE} -DSTDLIB_ALLOCATIONS -O2 -o ${BUILD_DIR}/${OUT} ${OBJ}

run:
	./${BUILD_DIR}/${OUT}

clean:
	rm -f ${BUILD_DIR}/* ${OBJ}