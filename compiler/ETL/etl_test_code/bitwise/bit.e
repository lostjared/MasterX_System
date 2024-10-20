#include<libetl/etl.e>

proc @andString(x, y) {
    let fmt = malloc (255);
    sprintf(fmt, "%d & %d = %x", x, y, x&y);
    return fmt;
}

proc init() {
    let and_s = andString(25, rand());
    puts(string(and_s));
    free(and_s);
    let u = 4242411111 >> 8 ^ 2;
    printf("U: %d\n", u);
    return 0;
}