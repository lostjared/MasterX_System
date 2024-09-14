
#include<libetl/etl.e>

proc init() {
    let x = 200;
    let y = 300;
    puts(str(x) + "+" + str(y) + "=" + str(x+y));
    puts(str(x * x));
    return 0;
}