OUTPUT=program
ETL=etl

.PHONY: all clean

$(OUTPUT):
	$(CXX) code.cpp -o code.o -c
	$(ETL) $(OUTPUT) hello.e code.o -- -lstdc++

all: $(OUTPUT)

clean:
	rm -f *.o $(OUTPUT)