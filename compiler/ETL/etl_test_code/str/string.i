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
# 3 "string.e" 2


# 4 "string.e"
proc init() {
    let x = 200;
    let y = 300;
    puts(str(x) + "+" + str(y) + "=" + str(x+y));
    puts(str(x * x));
    return 0;
}
