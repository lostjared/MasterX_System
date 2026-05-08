CPP_FILES := $(wildcard ../scanner/*.cpp) $(wildcard ../*.cpp) web.cpp
OBJ_FILES := $(notdir $(CPP_FILES:.cpp=.o))

CXXFLAGS := -std=c++20 -I../scanner/include -I../include -I../../include -sNO_DISABLE_EXCEPTION_CATCHING 

all: mxvm.js

%.o: ../scanner/%.cpp
	em++ $(CXXFLAGS) -c $< -o $@

%.o: ../%.cpp
	em++ $(CXXFLAGS) -c $< -o $@

web.o: web.cpp
	em++ $(CXXFLAGS) -c $< -o $@

mxvm.js: $(OBJ_FILES)
	em++ $(CXXFLAGS) $(OBJ_FILES) -o mxvm.js -lembind -sNO_DISABLE_EXCEPTION_CATCHING --preload-file ./include/io/io.mxvm --preload-file ./include/string/string.mxvm --preload-file ./include/sdl/sdl.mxvm --preload-file ./include/std/std.mxvm

clean:
	rm -f *.o *.wasm *.js *.data
