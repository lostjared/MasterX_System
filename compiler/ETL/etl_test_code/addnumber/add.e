#include<libetl/etl.e>

proc add_numbers() {
    puts("Enter first number: ");
    let x = scan_integer();
    puts("Enter second number: ");
    let y = scan_integer();
    let buffer = malloc ( 255 );
    sprintf(buffer, "\n%d+%d=%d\n", x, y, x+y);
    puts(string(buffer));
    free(buffer);
    return 0;
}

proc init() {
    let x = 0;
    let y = 0;
    add_numbers();
    return 0;
}


