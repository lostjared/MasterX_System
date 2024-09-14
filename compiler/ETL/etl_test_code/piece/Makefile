
OUTPUT=program
ETL=etl

all: 
	gcc `sdl2-config --cflags` sdl.c -o sdl.o -c
	$(ETL) $(OUTPUT) piece.e sdl.o -- `sdl2-config --libs`
clean:
	rm -f *.o $(OUTPUT)
