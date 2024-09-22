
OUTPUT=masterpiece
ETL=etl
SRC=piece.e

all: 
	$(CC) `sdl2-config --cflags` sdl.c -o sdl.o -c
	$(ETL) $(OUTPUT) $(SRC) sdl.o -- `sdl2-config --libs` -lSDL2_ttf
clean:
	rm -f *.o $(OUTPUT)
