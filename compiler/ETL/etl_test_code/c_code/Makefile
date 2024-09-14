OUTPUT=program
ETL=etl

.PHONY: all clean

$(OUTPUT):
	$(CC) code.c -o code.o -c
	$(ETL) $(OUTPUT) hello.e code.o 

all: $(OUTPUT)

clean:
	rm -f *.o $(OUTPUT)