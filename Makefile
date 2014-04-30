TARGET = main

CXX = mpic++
LD = mpic++

CXX_FLAGS = -Wall -Wextra
LD_FLAGS =

OUT_DIR = bin
SRC_DIR = src

.PHONY: all clean

all: ${OUT_DIR} ${OUT_DIR}/main.o
	${LD} -o ${OUT_DIR}/main ${OUT_DIR}/main.o ${LD_FLAGS}

${OUT_DIR}:
	mkdir -p ${OUT_DIR}

${OUT_DIR}/main.o: ${SRC_DIR}/main.cpp
	${CXX} -o ${OUT_DIR}/main.o -c ${SRC_DIR}/main.cpp ${CXX_FLAGS}

clean:
	-rm -rf bin/*.o
	-rm -rf bin/*.a
	-rm -rf bin/main
