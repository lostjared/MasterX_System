#include<libetl/etl.e>

proc $format($hello, x, y) {
    return hello + ": " + str(x) + "+" + str(y) + "=" + str(x+y);
}

proc init() {
    let x = 25;
    let y = 25;
    puts(format("Hey two numers: ", x, y));
    let s = "Jared Says: ";
    puts(format(s, 10, 10));
    return 0;
}
