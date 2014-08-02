CC=g++
CFLAGS=-g -Wall
SRC_DIR=src
BUILD_DIR=build
SRC_EXT=cpp
LIB_LINEAR=liblinear-1.94
LIB_BLAS = $(LIB_LINEAR)/blas/blas.a

SOURCE_FILES=$(wildcard $(SRC_DIR)/*.$(SRC_EXT)) $(wildcard $(LIB_LINEAR)/*.$(SRC_EXT))
#SOURCE_FILES=src/ActiveAndQLearn.cpp
OBJECT_FILES=$(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCE_FILES:.$(SRC_EXT)=.o)) 
OUT_EXE = ActiveAndQLearn
TARGET=bin
INC=-I include -I $(LIB_LINEAR)
#all:
#	@echo "$(OBJECT_FILES)"
#	@echo "$(SOURCE_FILES)"
$(TARGET): $(OBJECT_FILES)
	@echo "$(OBJECT_FILES)"
	@echo "$(SOURCE_FILES)"
	@mkdir -p $(TARGET)
	@echo "$(CC) $^ -o $(TARGET)/$(OUT_EXE) "; $(CC) $^ -o $(TARGET)/$(OUT_EXE)  $(LIB_BLAS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
	make -C liblinear-1.94
	@mkdir -p $(BUILD_DIR)
	@echo "$(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	make -C liblinear-1.94 clean
	-rm -r $(BUILD_DIR) $(TARGET)
