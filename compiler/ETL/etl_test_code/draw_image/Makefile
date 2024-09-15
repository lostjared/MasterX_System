
OUTPUT=draw_image
ETL=etl
SRC=image.e

all: 
	$(CC) `sdl2-config --cflags` sdl.c -o sdl.o -c
	$(ETL) $(OUTPUT) $(SRC) sdl.o -- `sdl2-config --libs`
clean:
	rm -f *.o $(OUTPUT)
