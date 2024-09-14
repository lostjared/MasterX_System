#include<libetl/etl.e>

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


