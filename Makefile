
CC = g++
LANG_SDT = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors
INCLUDE_PATH = -I"./libs/"

SRC_FILES = src/*.cpp \
			src/Game/*.cpp \
			src/Logger/*.cpp \
			src/ECS/*.cpp

LINKER_FLAGS = -lSDL2 \
				-lSDL2_image \
				-lSDL2_ttf \
				-lSDL2_mixer \
				-llua5.3

OBJ_NAME = gameengine

build:
	$(CC) $(COMPILER_FLAGS) $(LANG_sTD) $(INCLUDE_PATH) $(SRC_FILES) $(LINKER_FLAGS) -o $(OBJ_NAME)

run:
	./$(OBJ_NAME)

clean:
	rm $(OBJ_NAME)