OBJS = main.cpp logic.cpp

RESULT = Map_Colouring

FLAGS_DEBUG = -g -Wall #-Wl,--stack,2000000000

FLAGS_RELEASE = -O3 -Wl,-subsystem,windows

COMPILER64 = x86_64-w64-mingw32-g++

COMPILER32 = i686-w64-mingw32-g++

#begin with I
INCLUDES64 =  -IC:/msys64/mingw64/include \
			  -IC:/SDL_MinGW/SDL2-2.0.12/x86_64-w64-mingw32/include/SDL2 \
			  -ID:/C++/Image_Saver/include

INCLUDES32 =  -IC:/msys64/mingw32/include

#begin with L
LIBRARIES64 = -LC:/msys64/mingw64/lib \
			  -LC:/SDL_MinGW/SDL2-2.0.12/x86_64-w64-mingw32/lib \
			  -LD:/C++/Image_Saver/lib

LIBRARIES32 = -LC:/msys64/mingw32/lib

#begin with l
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -limagesaver

32 :
	mkdir 32
	mkdir 32\\debug
	mkdir 32\\release

64 :
	mkdir 64
	mkdir 64\\debug
	mkdir 64\\release

.PHONY : directories

directories : 32 64

debug_64 : directories
	$(COMPILER64) $(OBJS) $(FLAGS_DEBUG) $(INCLUDES64) $(LIBRARIES64) $(LINKER_FLAGS) -o 64/debug/$(RESULT)_64

debug_32 : directories
	$(COMPILER32) $(OBJS) $(FLAGS_DEBUG) $(INCLUDES32) $(LIBRARIES32) $(LINKER_FLAGS) -o 32/debug/$(RESULT)_32

release_64 : directories
	$(COMPILER64) $(OBJS) $(FLAGS_RELEASE) $(INCLUDES64) $(LIBRARIES64) $(LINKER_FLAGS) -o 64/release/$(RESULT)_64

release_32 : directories
	$(COMPILER32) $(OBJS) $(FLAGS_RELEASE) $(INCLUDES32) $(LIBRARIES32) $(LINKER_FLAGS) -o 32/release/$(RESULT)_32

all : directories
	$(COMPILER64) $(OBJS) $(FLAGS_DEBUG) $(INCLUDES) $(LIBRARIES) $(LINKER_FLAGS) -o 64/debug/$(RESULT)_64