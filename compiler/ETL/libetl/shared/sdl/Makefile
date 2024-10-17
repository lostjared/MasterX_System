all:
	gcc `sdl2-config --cflags` -O2  -c ../../sdl.c -o sdl.o -fPIC
	g++ `sdl2-config --cflags` -O2 sdl.o sdl.cpp -shared -o sdl.so -fPIC -lSDL2 -lSDL2_ttf
install: all
	cp ./sdl.so /usr/local/lib
clean:
	rm -f sdl.so *.o
