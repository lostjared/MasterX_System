# 0 "bit.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "bit.e"
# 1 "/usr/local/include/libetl/etl.e" 1 3




# 4 "/usr/local/include/libetl/etl.e" 3
define scan_integer();
define len($value);
define $str(value);
define exit(code);
define puts($output);
define srand(seed);
define time(value);
# 2 "bit.e" 2


# 3 "bit.e"
proc $andString(x, y) {
    return str(x) + "  & " + str(y) + " = " + str(x&y);
}

proc init() {
    puts(andString(10, 100));
    let u = 4242411111 >> 8 ^ 2;
    printf("%d\n", u);
    return 0;
}
