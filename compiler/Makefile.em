CXX=em++
CXX_FLAGS=-std=c++20 -I./include 
LDFLAGS= -lembind
CPP_FILES := export.cpp html.cpp scanner.cpp string_buffer.cpp types.cppp
OBJ_FILES := export.o html.o scanner.o string_buffer.o types.o
OUTPUT_NAME=etl.js

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
  
all: $(OBJ_FILES) 
	$(CXX) $(OBJ_FILES)  -o $(OUTPUT_NAME)  $(LDFLAGS)

clean:
	rm -f *.o 
