# 0 "number.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "number.e"



proc print_numbers() {
    let x = 25;
    let y = 100;
    let z = 50;
    let value = x*y/z+(2+4);
    printf("The value is: %d\n", value);
    return value;
}


proc init() {
    print_numbers();
    return 0;
}
