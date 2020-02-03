all: build_test
CC=emcc
PROJECT_SOURCE_FILES ?= src/main.c
#LDLIBS = -lraylib -framework OpenGL -framework OpenAL -framework Cocoa
LDLIBS = libs/libflecs_static.bc

RAYLIB_PATH        ?= /Users/pabloweremczuk/Documents/Proyectos/c/raylib
EMSDK_PATH          ?= C:/emsdk
EMSCRIPTEN_VERSION  ?= 1.38.31
CLANG_VERSION       = e$(EMSCRIPTEN_VERSION)_64bit
PYTHON_VERSION      = 2.7.13.1_64bit\python-2.7.13.amd64
NODE_VERSION        = 8.9.1_64bit
EMSCRIPTEN          = $(EMSDK_PATH)\emscripten\$(EMSCRIPTEN_VERSION)
CFLAGS = -O1 -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0
CFLAGS += -g4 -s USE_PTHREADS=1 -s USE_GLFW=3 -s TOTAL_MEMORY=67108864 --preload-file assets
#CFLAGS += --shell-file $(RAYLIB_PATH)/src/shell.html
INCLUDE_PATHS = -Iinclude

build_test: src/main.c
	$(CC) $(CFLAGS) $(PROJECT_SOURCE_FILES) $(LDLIBS) -o html/main.html $(INCLUDE_PATHS)

clean:
	rm ./html2/*