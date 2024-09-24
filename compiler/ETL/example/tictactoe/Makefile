
OUTPUT=tictactoe-ex
ETL=etl
SRC=tictactoe.e

all: 
	$(ETL) $(OUTPUT) $(SRC)  -- -L/usr/local/lib -letl_sdl -lSDL2 -lSDL2_ttf
clean:
	rm -f *.o *.i *.s $(OUTPUT)
