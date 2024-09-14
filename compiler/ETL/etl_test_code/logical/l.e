#include<libetl/etl.e>


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