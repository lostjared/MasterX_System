# 0 "length.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "length.e"
# 1 "/usr/local/include/libetl/etl.e" 1 3




# 4 "/usr/local/include/libetl/etl.e" 3
define scan_integer();
define len($value);
define $str(value);
define exit(code);
define puts($output);
define srand(seed);
define time(value);
define strlen($value);
define strcmp($value1, $value2);
define at($value, index);
# 2 "length.e" 2


# 3 "length.e"
proc testLen($what) {
    let n_s = "hey: " + what;
    puts(n_s);
    return len(n_s);
}

proc init() {
    puts("Len value:" + str(testLen("value is here")));
    return 0;
}
