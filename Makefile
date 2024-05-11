SRC = src/main.cpp src/entity.cpp src/player.cpp src/renderwindow.cpp src/shark.cpp
INCLUDES = -Iinclude
DLLS = SDL2.dll SDL2_image.dll SDL2_ttf.dll SDL2_mixer.dll
OUT_DIR = out

# Output files
OUT = $(OUT_DIR)/main.exe
OUT_JS = $(OUT_DIR)/index.html

# Compilers
CC = g++
EMCC = emcc

# Compilation flags for emsdk
EM_FLAGS = -O2 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS="['png']" -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 --preload-file res

# Determine the OS
ifeq ($(OS),Windows_NT)
    RM = del /Q
    RMDIR = rmdir /S /Q
    COPY = copy
    XCOPY = xcopy
else
    RM = rm -f
    RMDIR = rm -rf
    COPY = cp
    XCOPY = cp -r
endif

all: $(OUT) copy-dlls run

$(OUT): $(SRC)
	$(CC) $(SRC) $(INCLUDES) -Llib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -o $(OUT)

emsdk: $(OUT_JS)

$(OUT_JS): $(SRC)
	$(EMCC) $(SRC) $(INCLUDES) $(EM_FLAGS) -o $(OUT_JS)

copy-dlls:
	$(foreach DLL, $(DLLS), $(COPY) $(DLL) $(OUT_DIR) &)
	$(XCOPY) res $(OUT_DIR)\res /E /I /Y

run:
	$(OUT_DIR)/main.exe

clean:
	-$(RM) $(OUT_DIR)\*.exe
	-$(RM) $(OUT_DIR)\*.dll
	-$(RM) $(OUT_DIR)\*.js $(OUT_DIR)\*.wasm $(OUT_DIR)\*.data $(OUT_DIR)\*.html
	-$(RMDIR) $(OUT_DIR)\res
	@echo Cleaned
