
#include<libetl/etl.e>

#define MACRO_STRING "Hey this is a number: "
#define MUL(x, y) x*y


proc init() {
    puts(MACRO_STRING + str(MUL(10, 10)));
    return 0;
}