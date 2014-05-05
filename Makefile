TARGET = main

CXX = mpic++
LD = mpic++
AR = ar

CXX_FLAGS = -Wall -Wextra
LD_FLAGS =
AR_FLAGS = rs

OUT_DIR = bin
SRC_DIR = src

.PHONY: all clean

all: ${OUT_DIR} ${OUT_DIR}/main.o ${OUT_DIR}/thief.a
	${LD} -o ${OUT_DIR}/main ${OUT_DIR}/main.o ${OUT_DIR}/thief.a

${OUT_DIR}:
	mkdir -p ${OUT_DIR}

${OUT_DIR}/main.o: ${SRC_DIR}/main.cpp ${SRC_DIR}/thief_process.h ${SRC_DIR}/sizes.h
	${CXX} -o ${OUT_DIR}/main.o -c ${SRC_DIR}/main.cpp ${CXX_FLAGS}

${OUT_DIR}/thief.a: ${OUT_DIR}/thief_process.o ${OUT_DIR}/waiting_process.o ${OUT_DIR}/waiting_queue.o ${OUT_DIR}/logger.o
	${AR} ${AR_FLAGS} ${OUT_DIR}/thief.a ${OUT_DIR}/thief_process.o ${OUT_DIR}/waiting_process.o ${OUT_DIR}/waiting_queue.o ${OUT_DIR}/logger.o

${OUT_DIR}/thief_process.o: ${SRC_DIR}/thief_process.cpp ${SRC_DIR}/thief_process.h ${SRC_DIR}/sizes.h ${SRC_DIR}/waiting_queue.h ${SRC_DIR}/logger.h
	${CXX} -o ${OUT_DIR}/thief_process.o -c ${SRC_DIR}/thief_process.cpp ${CXX_FLAGS}

${OUT_DIR}/waiting_process.o: ${SRC_DIR}/waiting_process.cpp ${SRC_DIR}/waiting_process.h ${SRC_DIR}/waiting_queue.h
	${CXX} -o ${OUT_DIR}/waiting_process.o -c ${SRC_DIR}/waiting_process.cpp ${CXX_FLAGS}

${OUT_DIR}/waiting_queue.o: ${SRC_DIR}/waiting_queue.cpp ${SRC_DIR}/waiting_queue.h ${SRC_DIR}/waiting_process.h
	${CXX} -o ${OUT_DIR}/waiting_queue.o -c ${SRC_DIR}/waiting_queue.cpp ${CXX_FLAGS}

${OUT_DIR}/logger.o: ${SRC_DIR}/logger.cpp ${SRC_DIR}/logger.h ${SRC_DIR}/thief_process.h ${SRC_DIR}/sizes.h
	${CXX} -o ${OUT_DIR}/logger.o -c ${SRC_DIR}/logger.cpp ${CXX_FLAGS}

clean:
	-rm -rf bin/*.o
	-rm -rf bin/*.a
	-rm -rf bin/main
