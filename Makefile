# external parameters
export DEBUG = 1
export OPT_LEVEL = 1

# judge if is debug mode
ifeq ($(DEBUG), 0)
	DEBUG_ARG = 
	OPT_ARG = -O$(OPT_LEVEL)
else
	DEBUG_ARG = -DDEBUG -g
	OPT_ARG = 
endif

# directories
export TOP_DIR = $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)
TARGET_DIR = $(TOP_DIR)/build
OBJ_DIR = $(TOP_DIR)/build/obj
PL01_OBJ_DIR = $(OBJ_DIR)/pl01
TEST_OBJ_DIR = $(OBJ_DIR)/test
SRC_DIR = $(TOP_DIR)/src
TEST_DIR = $(TOP_DIR)/test
INCLUDE_ARG := -I$(SRC_DIR)/inc -I$(TEST_DIR) -I/usr/local/opt/llvm/include
LIBRARY_ARG := -L/usr/local/opt/llvm/lib -lLLVMCore -lLLVMBinaryFormat
LIBRARY_ARG += -lLLVMSupport -lLLVMDemangle -lz -lcurses -lm -lxml2

# files
PL01_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(PL01_OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))
PL01_OBJS += $(patsubst $(SRC_DIR)/%.cpp, $(PL01_OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/**/*.cpp))
PL01_OBJS += $(patsubst $(SRC_DIR)/%.cpp, $(PL01_OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/**/**/*.cpp))
TEST_OBJS := $(patsubst $(TEST_DIR)/%.cpp, $(TEST_OBJ_DIR)/%.o, $(wildcard $(TEST_DIR)/*.cpp))
TEST_OBJS += $(patsubst $(TEST_DIR)/%.cpp, $(TEST_OBJ_DIR)/%.o, $(wildcard $(TEST_DIR)/**/*.cpp))
TEST_OBJS += $(filter-out $(PL01_OBJ_DIR)/main.o, $(PL01_OBJS))
PL01_TARGET := $(TARGET_DIR)/pl01
TEST_TARGET := $(TARGET_DIR)/test

# compiler & linker
# NOTE: only support macOS & Debian
OS := $(shell uname -s)
ifeq ($(OS), Darwin)
	include make/macos.make
else
	include make/debian.make
endif

.PHONY: all pl01 test clean

all: pl01 test

pl01: $(TARGET_DIR) $(PL01_OBJ_DIR) $(PL01_TARGET)

test: $(TARGET_DIR) $(TEST_OBJ_DIR) $(TEST_TARGET)

clean:
	-rm -rf $(PL01_OBJ_DIR)
	-rm -rf $(TEST_OBJ_DIR)
	-rm -f $(PL01_TARGET)
	-rm -f $(TEST_TARGET)

# directories
$(TARGET_DIR):
	mkdir $(TARGET_DIR)

$(PL01_OBJ_DIR):
	mkdir -p $(PL01_OBJ_DIR)

$(TEST_OBJ_DIR):
	mkdir -p $(TEST_OBJ_DIR)

# PL/0.1
$(PL01_TARGET): $(PL01_OBJS)
	$(LD) -o $@ $^

$(PL01_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	-mkdir -p $(dir $@)
	$(CC) -o $@ $^

# unit test
$(TEST_TARGET): $(TEST_OBJS)
	$(LD) -o $@ $^

$(TEST_OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	-mkdir -p $(dir $@)
	$(CC) -o $@ $^
