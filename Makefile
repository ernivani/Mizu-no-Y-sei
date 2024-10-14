SRC = src/main.cpp src/entity.cpp src/player.cpp src/renderwindow.cpp src/shark.cpp

INCLUDES = -Iinclude

OUT_DIR = out

CC = g++
EMCC = emcc

EM_FLAGS = -O2 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS="['png']" -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 --preload-file res

ifeq ($(OS),Windows_NT)
    RM = del /Q
    RMDIR = rmdir /S /Q
    COPY = copy
    XCOPY = xcopy
    SEP = \\
    EXE = .exe
    DLLS = SDL2.dll SDL2_image.dll SDL2_ttf.dll SDL2_mixer.dll
    OUT = $(OUT_DIR)$(SEP)main$(EXE)
    RUN_CMD = $(OUT)
    LIB_DIRS = -Llib
    LIBS = $(LIB_DIRS) -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
    COPY_DLLS = $(foreach DLL, $(DLLS), $(COPY) $(DLL) $(OUT_DIR) &)
    COPY_RES = $(XCOPY) res $(OUT_DIR)$(SEP)res /E /I /Y
else
    RM = rm -f
    RMDIR = rm -rf
    COPY = cp
    XCOPY = cp -r
    SEP = /
    EXE =
    DLLS =
    OUT = $(OUT_DIR)$(SEP)main$(EXE)
    RUN_CMD = ./$(OUT)
    LIB_DIRS =
    LIBS = $(LIB_DIRS) -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
    COPY_DLLS =
    COPY_RES = $(XCOPY) res $(OUT_DIR)$(SEP)res
endif

all: $(OUT) copy-resources run

$(OUT): $(SRC)
	$(CC) $(SRC) $(INCLUDES) $(LIBS) -o $(OUT)

emsdk: $(OUT_JS)

$(OUT_JS): $(SRC)
	$(EMCC) $(SRC) $(INCLUDES) $(EM_FLAGS) -o $(OUT_JS)

copy-resources:
	$(COPY_DLLS)
	$(COPY_RES)

run:
	$(RUN_CMD)

clean:
	-$(RM) $(OUT_DIR)$(SEP)*$(EXE)
	-$(RM) $(OUT_DIR)$(SEP)*.dll
	-$(RM) $(OUT_DIR)$(SEP)*.js $(OUT_DIR)$(SEP)*.wasm $(OUT_DIR)$(SEP)*.data $(OUT_DIR)$(SEP)*.html
	-$(RMDIR) $(OUT_DIR)$(SEP)res
	@echo Cleaned
