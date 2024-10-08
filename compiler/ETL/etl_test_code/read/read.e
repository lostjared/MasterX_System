#include<libetl/etl.e>
#include<libetl/io.e>


proc init() {
	let file = file_open("test.txt", "rb");
	let len = file_size(file);
	let data = malloc(len + 1);
	file_read(file, data, len);
	memstoreb(data, len, 0);
	printf("%s\n", data);
	file_close(file);
	return 0;
}
