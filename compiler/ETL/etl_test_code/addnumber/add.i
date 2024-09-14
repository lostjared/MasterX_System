# 0 "add.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "add.e"
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
# 2 "add.e" 2


# 3 "add.e"
proc add_numbers() {
    puts("Enter firsT number: ");
    let x = scan_integer();
    puts("Enter second number: ");
    let y = scan_integer();
    puts(str(x) + "+" + str(y) + "=" + str(x+y));
     return 0;
}

proc init() {
    let x = 0;
    let y = 0;
    add_numbers();
    return 0;
}
