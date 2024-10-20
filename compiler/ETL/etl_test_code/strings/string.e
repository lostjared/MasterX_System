#include<libetl/etl.e>

proc @format($hello, x, y) {
    let fmt = malloc (255 + (22*4));
    sprintf(fmt, "%s: %d+%d=%d", hello, x, y, x+y);
    return fmt;
}

proc init() {
    let x = 25;
    let y = 25;
    let fmt = format("Hey two numbers: ", x, y);
    puts(string(fmt));
    free(fmt);
    return 0;
}
