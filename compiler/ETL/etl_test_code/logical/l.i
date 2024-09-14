# 0 "l.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "l.e"
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
# 2 "l.e" 2



# 4 "l.e"
proc init() {
    let x = 2;
    let y = 4;
    if(x == 2 && y == 4) {
        printf("Yup\n");
    } else {
        printf("Nope");
    }
    while (x <= 10 && y == 4) {
        printf("%d\n", x);
        x = x + 1;
    }
    if(x < 11 || x == 11) {
        printf("true\n");
    }
    return 0;
}
