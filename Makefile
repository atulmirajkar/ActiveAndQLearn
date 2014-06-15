CC=g++
CFLAGS=-g -Wall
SRC_DIR=src
BUILD_DIR=build
SRC_EXT=cpp

SOURCE_FILES=$(wildcard $(SRC_DIR)/*.$(SRC_EXT))
#SOURCE_FILES=src/ActiveAndQLearn.cpp
OBJECT_FILES=$(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(SOURCE_FILES:.$(SRC_EXT)=.o)) 
OUT_EXE = ActiveAndQLearn
TARGET=bin
INC=-I include
#all:
#	@echo "$(OBJECT_FILES)"
#	@echo "$(SOURCE_FILES)"
$(TARGET): $(OBJECT_FILES)
	@echo "$(OBJECT_FILES)"
	@echo "$(SOURCE_FILES)"
	@mkdir -p $(TARGET)
	@echo "$(CC) $^ -o $(TARGET)/$(OUT_EXE)"; $(CC) $^ -o $(TARGET)/$(OUT_EXE)  

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
	@mkdir -p $(BUILD_DIR)
	@echo "$(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	-rm -r $(BUILD_DIR) $(TARGET)
