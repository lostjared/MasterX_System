
OUTPUT=masterpiece
ETL=etl
SRC=piece.e

all: 
	$(ETL) $(OUTPUT) $(SRC) -- -L/usr/local/lib -letl_sdl -lSDL2 -lSDL2_ttf
clean:
	rm -f *.o *.i *.s $(OUTPUT)  
