CC := g++
CFLAGS := -Wall -o0 -g

#ifeq ($(DEBUG), 1)
#       CFLAGS := $(CFLAGS) -O0 -g
#else
#        CFLAGS := $(CFLAGS) -O2
#endif

SRC_DIR := ./
BIN_DIR := bin
OBJ_DIR := obj
TRUNK := ../

C_FILES := $(SRC_DIR)/elf_parser/elf_parser.cpp $(SRC_DIR)/func_instr/func_instr.cpp $(SRC_DIR)/func_memory/func_memory.cpp $(SRC_DIR)/func_sim.cpp $(SRC_DIR)/main.cpp

OBJS_FILES := ${C_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o}
INCL := -I $(SRC_DIR) -I $(TRUNK)/common/ -I $(SRC_DIR)/elf_parser/ -I $(SRC_DIR)/func_memory/ -I $(SRC_DIR)/func_instr/

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
		$(CC) $(CFLAGS) $(INCL) $< -c -o $@

$(BIN_DIR)/program: $(OBJS_FILES)
		$(CC) $(CFLAGS) $(LDFLAGS) $(INCL) $^ -o $@ -l elf

Debug: all

all: build_dirs $(BIN_DIR)/program

build_dirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)/elf_parser
	mkdir -p $(OBJ_DIR)/elf_parser
	mkdir -p $(BIN_DIR)/func_memory
	mkdir -p $(OBJ_DIR)/func_memory
	mkdir -p $(BIN_DIR)/func_instr
	mkdir -p $(OBJ_DIR)/func_instr

clean:
	rm -rf $(BIN_DIR)
	rm -rf $(OBJ_DIR)
	rm -f $(BIN_DIR)/program
