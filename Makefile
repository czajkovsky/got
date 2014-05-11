TARGET = main
THIEF_LIB = libthief.a

CXX = mpic++
LD = mpic++
AR = ar

CXX_FLAGS = -Wall -Wextra
LD_FLAGS =
AR_FLAGS = rs

OBJ_DIR = bin
SRC_DIR = src

MAIN_SRCS = main.cpp
THIEF_LIB_SRCS =  communicator.cpp \
									ender.cpp \
									left_house.cpp \
									logger.cpp \
									message.cpp \
									thief_process.cpp \
									time_point.cpp \
									waiting_process.cpp \
									waiting_process_priority_queue.cpp
MAIN_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(MAIN_SRCS:.cpp=.o)))
THIEF_LIB_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(THIEF_LIB_SRCS:.cpp=.o)))

INCLUDES = $(wildcard $(SRC_DIR)/*.h)

USER_MESSAGE = "Welcome to the Guild of Thieves!"

.PHONY: all clean

all: $(OBJ_DIR) $(OBJ_DIR)/$(TARGET)
	@echo $(USER_MESSAGE)

$(OBJ_DIR)/$(TARGET): $(MAIN_OBJS) $(OBJ_DIR)/$(THIEF_LIB)
	$(LD) $(LD_FLAGS) -o $(OBJ_DIR)/$(TARGET) $^

$(OBJ_DIR)/$(THIEF_LIB): $(THIEF_LIB_OBJS)
	$(AR) $(AR_FLAGS) $(OBJ_DIR)/$(THIEF_LIB) $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDES)
	$(CXX) $(CXX_FLAGS) -o $@ -c $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	-rm -rf $(OBJ_DIR)/*.o
	-rm -rf $(OBJ_DIR)/*.a
	-rm -rf $(OBJ_DIR)/$(TARGET)
