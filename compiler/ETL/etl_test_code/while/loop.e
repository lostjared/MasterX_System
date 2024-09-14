#include<libetl/etl.e>

proc init() {
    let x = "Hello, World";
    let i = 0;
    let length = strlen(x);
    while(i < length) {
        let value = at(x, i);
        printf("Letter is: %c at index: %d\n", value, i);
        i = i + 1;
    }
    return 0;
}

