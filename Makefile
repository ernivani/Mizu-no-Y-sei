SRC = src/main.cpp src/entity.cpp src/player.cpp src/renderwindow.cpp
INCLUDES = -I include -L lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
OUT = out/main.exe
CC = g++

all:
	$(CC) $(SRC) $(INCLUDES) -o $(OUT)
	./$(OUT)
