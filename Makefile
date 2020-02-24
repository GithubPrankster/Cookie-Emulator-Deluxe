CXXFLAGS = -std=c++17 -Ofast -fopenmp -march=native -mtune=native
WARNFLAGS = -Wall -Wextra -W

SDLLINK = -lSDL2main -lSDL2

all: 
	$(CXX) $(CXXFLAGS) $(WARNFLAGS) main.cpp $(SDLLINK) -o ckemu
	
	
	
