
#include<libetl/etl.e>

#define MACRO_STRING "Hey this is a number: %d\n"
#define MUL(x, y) x*y


proc init() {
    printf(MACRO_STRING, MUL(10, 10));
    return 0;
}