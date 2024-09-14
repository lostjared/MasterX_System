CXX=em++
CXX_FLAGS=-std=c++20 -I./include -I/usr/local/include/cxx_scan -sNO_DISABLE_EXCEPTION_CATCHING
LDFLAGS= -lembind -sNO_DISABLE_EXCEPTION_CATCHING
CPP_FILES := export.cpp debug.cpp ir.cpp test.cpp parser.cpp 
OBJ_FILES := export.o debug.o ir.o test.o parser.o ../scanner.o ../types.o ../string_buffer.o
OUTPUT_NAME=webapp/etl-ir.js

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
  
all: $(OBJ_FILES) 
	$(CXX) $(OBJ_FILES)  -o $(OUTPUT_NAME)  $(LDFLAGS)

clean:
	rm -f *.o 
