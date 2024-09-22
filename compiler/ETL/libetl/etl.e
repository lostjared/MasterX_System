#ifndef __ETL__H__
#define __ETL__H__
define scan_integer();
define len($value);
define $str(value);
define exit(code);
define puts($output);
define srand(seed);
define rand();
define time(value);
define strlen($value);
define strcmp($value1, $value2);
define at($value, index);
define @malloc(bytes);
define @calloc(bytes, size);
define free(@memory);
define mematl(@memory, index);
define mematb(@memory, index);
define memclr(@memory, size);
define memstorel(@memory, index, value);
define memstoreb(@memory, index, value);
define @meminsert(@buffer, index, value, element, @current_size);
define @memremove(@buffer,index, element, @current_size);
define @memmove(@dest, @src, size);
define @memcpy(@dest, @src, size);
#endif
