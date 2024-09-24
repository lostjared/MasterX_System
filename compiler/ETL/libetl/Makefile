
ETL_PATH=/usr/local

all:
	$(CC) libetl.c -o libetl.o -c
	$(CC) `sdl2-config --cflags` -O2 sdl.c -o sdl.o -c
	$(AR) cr libetl.a libetl.O
	$(AR) cr libetl_sdl.a sdl.O
install:
	cp -rf *.e $(ETL_PATH)/include/libetl/
	cp -rf *.a $(ETL_PATH)/lib/
clean:
	rm -f *.o
