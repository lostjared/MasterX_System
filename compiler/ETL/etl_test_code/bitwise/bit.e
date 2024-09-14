#include<libetl/etl.e>

proc $andString(x, y) {
    return str(x) + "  & " + str(y) + " = " + str(x&y);
}

proc init() {
    puts(andString(10, 100));
    let u = 4242411111 >> 8 ^ 2;
    printf("%d\n", u);
    return 0;
}