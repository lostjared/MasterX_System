CPP_FILES := $(wildcard ../../scanner/*.cpp) $(wildcard ../../*.cpp) syntax.cpp
OBJ_FILES := $(notdir $(CPP_FILES:.cpp=.o))

CXXFLAGS := -std=c++20 -O3 -I../../scanner/include -I../../include -I../../../include -s NO_DISABLE_EXCEPTION_CATCHING 
 

all: mxvm-html.js

%.o: ../../scanner/%.cpp
	em++ $(CXXFLAGS) -c $< -o $@

%.o: ../../%.cpp
	em++ $(CXXFLAGS) -c $< -o $@

syntax.o: syntax.cpp
	em++ $(CXXFLAGS) -c $< -o $@

mxvm-html.js: $(OBJ_FILES)
	em++ $(CXXFLAGS) $(OBJ_FILES) -o mxvm-html.js -lembind -s NO_DISABLE_EXCEPTION_CATCHING -s MODULARIZE=1 -s EXPORT_ES6=1 -s ENVIRONMENT=web 

clean:
	rm -f *.o *.wasm *.js *.data
