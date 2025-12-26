
CXX = em++
CXXFLAGS = -std=c++20 -O2 -DFOR_WASM -s USE_SDL=2 -s USE_SDL_TTF=2 
#set to libs path
LIBS_PATH=$(HOME)/emscripten-libs
ZLIB_INCLUDE= -s USE_ZLIB=1
PNG_INCLUDE= -s USE_LIBPNG=1 #-I${LIBS_PATH}/libpng/include
ZLIB_LIB= -s USE_ZLIB=1 #$(LIBS_PATH)/zlib/lib/libz.a
PNG_LIB= -s USE_LIBPNG=1  # $(LIBS_PATH)/libpng/lib/libpng.a


OUTPUT = MasterX.html
SOURCES = src/dimension.cpp src/main.cpp src/splash.cpp src/window.cpp src/terminal.cpp src/mx_controls.cpp src/mx_window.cpp src/mx_system_bar.cpp src/mx_event.cpp src/loadpng.cpp src/masterpiece.cpp src/cfg.cpp src/matrix.cpp src/mx_menu.cpp src/asteroid_window.cpp src/tetris_window.cpp src/messagebox.cpp src/pac_window.cpp src/pong_window.cpp
OBJECTS = $(SOURCES:.cpp=.o)
PRELOAD = --preload-file assets

all: $(OUTPUT)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(ZLIB_INCLUDE) $(PNG_INCLUDE) -c $< -o $@

$(OUTPUT): $(OBJECTS)
	make -C ./src/apps/ats
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(OUTPUT) $(PRELOAD)  ./src/apps/ats/libats.a -s USE_SDL=2 -s USE_SDL_TTF=2 $(PNG_LIB) $(ZLIB_LIB) -s ALLOW_MEMORY_GROWTH -s ASSERTIONS -s ENVIRONMENT=web

clean:
	make -C ./src/apps/ats clean
	rm -f $(OUTPUT) $(OBJECTS)
