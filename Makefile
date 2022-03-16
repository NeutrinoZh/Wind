.PHONY: clean

CC = g++
FLAGS = -MD -Wall -I ./include -O3 $(compile_flags)

SRC_DIR = ./src

DIRS := $(SRC_DIR) $(addprefix $(SRC_DIR)/, $(SUB_DIR)) 
SRC := $(wildcard $(addsuffix /*.cpp, $(DIRS)))
OBJ := $(patsubst $(SRC_DIR)/%.cpp, bin/$(configure)/objs/%.o, $(SRC))

build: directory $(OBJ)
	$(CC) $(FLAGS) -o ./bin/$(configure)/main.exe $(OBJ)

directory: 
	@mkdir -p bin/$(configure)/objs $(addprefix bin/$(configure)/objs/, $(SUB_DIR))

VPATH := $(SRC_DIR)/
bin/$(configure)/objs/%.o : %.cpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf ./bin/$(configure)/main ./bin/$(configure)/objs/

include $(wildcard $(addsuffix /*.d, ./ ./$(SUB_DIR)/))