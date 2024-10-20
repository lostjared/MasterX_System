
#include<libetl/etl.e>

proc init() {
    let x = 200;
    let y = 300;
    let fmt = malloc(255);
    sprintf(fmt, "%d+%d=%d", x, y, x+y);
    puts(string(fmt));
    free(fmt);
    return 0;
}