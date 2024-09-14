# 0 "loop.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "loop.e"
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
# 2 "loop.e" 2


# 3 "loop.e"
proc init() {
    let x = "Hello, World";
    let i = 0;
    let length = strlen(x);
    while(i < length) {
        let value = at(x, i);
        printf("Letter is: %c at index: %d\n", value, i);
        i = i + 1;
    }
    return 0;
}
