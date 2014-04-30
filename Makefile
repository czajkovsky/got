TARGET = main

CXX = mpic++
LD = mpic++

CXX_FLAGS = -Wall -Wextra
LD_FLAGS =

OUT_DIR = bin
SRC_DIR = src

.PHONY: all clean

all: ${OUT_DIR} ${OUT_DIR}/main.o ${OUT_DIR}/thief_process.o
	${LD} -o ${OUT_DIR}/main ${OUT_DIR}/main.o ${OUT_DIR}/thief_process.o ${LD_FLAGS}

${OUT_DIR}:
	mkdir -p ${OUT_DIR}

${OUT_DIR}/main.o: ${SRC_DIR}/main.cpp ${SRC_DIR}/thief_process.h
	${CXX} -o ${OUT_DIR}/main.o -c ${SRC_DIR}/main.cpp ${CXX_FLAGS}

${OUT_DIR}/thief_process.o: ${SRC_DIR}/thief_process.cpp ${SRC_DIR}/thief_process.h
	${CXX} -o ${OUT_DIR}/thief_process.o -c ${SRC_DIR}/thief_process.cpp ${CXX_FLAGS}

clean:
	-rm -rf bin/*.o
	-rm -rf bin/*.a
	-rm -rf bin/main
