
OUTPUT=hello-ex
ETL=etl
SRC=hello.e

all: 
	$(ETL) $(OUTPUT) -q $(SRC) -- -L/usr/local/lib -letl_sdl -lSDL2 -lSDL2_ttf
clean:
	rm -f *.o *.s *.i $(OUTPUT)
