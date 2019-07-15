CXX = g++
CXXFLAGS = -std=c++2a -O3 -fopenmp
WARNFLAGS = -Wall -Wextra -W

SDLINC = -IC:/miscDev/devlibs/SDL2-devel-2.0.9-mingw/SDL2-2.0.9/x86_64-w64-mingw32/include/SDL2
SDLLIB = -LC:/miscDev/devlibs/SDL2-devel-2.0.9-mingw/SDL2-2.0.9/x86_64-w64-mingw32/lib
SDLLINK = -lmingw32 -lSDL2main -lSDL2 -mwindows -mconsole

all: 
	$(CXX) $(CXXFLAGS) $(WARNFLAGS) main.cpp $(SDLINC) $(SDLLIB) $(SDLLINK) -o ckemu
	
	
	