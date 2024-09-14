#include<libetl/etl.e>

proc testLen($what) {
    let n_s = "hey: " + what;
    puts(n_s);
    return len(n_s);
}

proc init() {
    puts("Len value:" + str(testLen("value is here")));
    return 0;
}