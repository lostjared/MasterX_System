# 0 "cmp.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "cmp.e"
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
# 2 "cmp.e" 2


# 3 "cmp.e"
proc init() {
    let v = "Hello World";
    printf("value is: %d\n", strlen(v));
    let x = strlen(v) > 0 && strcmp(v, "Hello World") == 0;
    puts("Comparison: " + str(x));
    return 0;
}
