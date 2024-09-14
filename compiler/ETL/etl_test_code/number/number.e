#define OUTPUT "The Value is: %d"
#define ADD(x,y) x+y

#include<libetl/etl.e>

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
