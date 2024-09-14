
CXX=em++
AR=emar
CXX_FLAGS=-O2 -std=c++20 -I./include 
LDFLAGS= -lembind
CPP_FILES := $(wildcard *.cpp)
OBJ_FILES := $(addprefix ,$(notdir $(CPP_FILES:.cpp=.o)))
OUTPUT_NAME=./ats.a

%.o: %.cpp
	$(CXX) $(CXX_FLAGS) -c -o $@ $<
  
all: $(OBJ_FILES) 
	$(AR) cru ./libats.a $(OBJ_FILES);

clean:
	rm -f *.o libats.a