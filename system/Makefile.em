
CXX = em++
CXXFLAGS = -std=c++20 -O2 -DFOR_WASM -s USE_SDL=2 -s USE_SDL_TTF=2 
OUTPUT = MasterX.html
SOURCES = src/dimension.cpp src/main.cpp src/splash.cpp src/window.cpp src/terminal.cpp src/mx_controls.cpp src/mx_window.cpp src/mx_system_bar.cpp src/mx_event.cpp
OBJECTS = $(SOURCES:.cpp=.o)
PRELOAD = --preload-file assets

all: $(OUTPUT)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OUTPUT): $(OBJECTS)
	make -C ./src/apps/ats
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(OUTPUT) $(PRELOAD)  ./src/apps/ats/libats.a -s USE_SDL=2 -s USE_SDL_TTF=2 -s ALLOW_MEMORY_GROWTH -s ASSERTIONS -s ENVIRONMENT=web

clean:
	make -C ./src/apps/ats clean
	rm -f $(OUTPUT) $(OBJECTS)
