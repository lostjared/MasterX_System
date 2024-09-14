# 0 "add.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "add.e"
proc add_numbers() {
    puts("Enter firsT number: ");
    let x = scan_integer();
    puts("Enter second number: ");
    let y = scan_integer();
    puts(str(x) + "+" + str(y) + "=" + str(x+y));
     return 0;
}

proc init() {
    add_numbers();
    return 0;
}
