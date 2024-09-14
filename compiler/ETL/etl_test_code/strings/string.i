# 0 "string.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "string.e"
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
# 2 "string.e" 2


# 3 "string.e"
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
