#include<libetl/etl.e>

proc init() {
    let v = "Hello World";
    printf("value is: %d\n", strlen(v));
    let x = strlen(v) > 0 && strcmp(v, "Hello World") == 0;
    puts("Comparison: " + str(x));
    return 0;
}
