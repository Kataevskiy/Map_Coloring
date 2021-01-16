OBJS = main.cpp

RESULT = build/debug_build_64

COMPILER = x86_64-w64-mingw32-g++

FLAGS = -g -Wall 

#begin with I
INCLUDES =  -IC:/msys64/mingw64/include \
			-IC:/SDL_MinGW/SDL2-2.0.12/x86_64-w64-mingw32/include/SDL2 \
			-ID:/C++/Image_Saver/include

#begin with L
LIBRARIES = -LC:/msys64/mingw64/lib \
			-LC:/SDL_MinGW/SDL2-2.0.12/x86_64-w64-mingw32/lib \
			-LD:/C++/Image_Saver/lib

#begin with l
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -limagesaver

all :
	$(COMPILER) $(OBJS) $(FLAGS) $(INCLUDES) $(LIBRARIES) $(LINKER_FLAGS) -o $(RESULT)