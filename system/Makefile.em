
CXX = em++
CXXFLAGS = -std=c++20 -O2 -DFOR_WASM -s USE_SDL=2 -s USE_SDL_TTF=2 -s NO_DISABLE_EXCEPTION_CATCHING -MMD -MP
LDFLAGS = -s ASYNCIFY -s ASYNCIFY_STACK_SIZE=65536 -s 'ASYNCIFY_IMPORTS=["emscripten_sleep"]'
#set to libs path
LIBS_PATH=$(HOME)/emscripten-libs
ZLIB_INCLUDE= -s USE_ZLIB=1
PNG_INCLUDE= -s USE_LIBPNG=1 #-I${LIBS_PATH}/libpng/include
ZLIB_LIB= -s USE_ZLIB=1 #$(LIBS_PATH)/zlib/lib/libz.a
PNG_LIB= -s USE_LIBPNG=1  # $(LIBS_PATH)/libpng/lib/libpng.a

# CMD library sources
CMD_PATH = ../cmd
CMD_INCLUDE = -I$(CMD_PATH)/include

OUTPUT = MasterX.html
SOURCES = src/dimension.cpp src/main.cpp src/splash.cpp src/window.cpp src/terminal.cpp src/mx_controls.cpp src/mx_window.cpp src/mx_system_bar.cpp src/mx_event.cpp src/loadpng.cpp src/masterpiece.cpp src/cfg.cpp src/matrix.cpp src/mx_menu.cpp src/asteroid_window.cpp src/tetris_window.cpp src/messagebox.cpp src/pac_window.cpp src/pong_window.cpp
CMD_SOURCES = src/apps/cmd/cmd_shell.cpp 
CMD_LIB_SOURCES = $(CMD_PATH)/src/command.cpp $(CMD_PATH)/src/scanner.cpp $(CMD_PATH)/src/string_buffer.cpp $(CMD_PATH)/src/types.cpp $(CMD_PATH)/src/ast.cpp $(CMD_PATH)/src/html.cpp $(CMD_PATH)/src/command_reg.cpp
OBJECTS = $(SOURCES:.cpp=.o)
CMD_OBJECTS = $(CMD_SOURCES:.cpp=.o)
CMD_LIB_OBJECTS = $(CMD_LIB_SOURCES:.cpp=.o)
PRELOAD = --preload-file assets

all: $(OUTPUT)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(CMD_INCLUDE) $(ZLIB_INCLUDE) $(PNG_INCLUDE) -c $< -o $@

$(CMD_PATH)/src/%.o: $(CMD_PATH)/src/%.cpp
	$(CXX) $(CXXFLAGS) $(CMD_INCLUDE) $(ZLIB_INCLUDE) $(PNG_INCLUDE) -c $< -o $@

$(OUTPUT): $(OBJECTS) $(CMD_OBJECTS) $(CMD_LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJECTS) $(CMD_OBJECTS) $(CMD_LIB_OBJECTS) -o $(OUTPUT) $(PRELOAD) -s USE_SDL=2 -s USE_SDL_TTF=2 $(PNG_LIB) $(ZLIB_LIB) -s ALLOW_MEMORY_GROWTH -s ASSERTIONS -s ENVIRONMENT=web -s NO_DISABLE_EXCEPTION_CATCHING -s EXPORTED_FUNCTIONS="['_main','_forceFrameRender']" -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']"

clean:
	rm -f $(OUTPUT) $(OBJECTS) $(CMD_OBJECTS) $(CMD_LIB_OBJECTS) src/apps/cmd/*.o \
		$(OBJECTS:.o=.d) $(CMD_OBJECTS:.o=.d) $(CMD_LIB_OBJECTS:.o=.d)

-include $(OBJECTS:.o=.d) $(CMD_OBJECTS:.o=.d) $(CMD_LIB_OBJECTS:.o=.d)