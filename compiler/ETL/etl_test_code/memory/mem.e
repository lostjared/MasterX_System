#include<libetl/etl.e>

proc @allocate_array(size) {
    let x = malloc(size+1);
    memclr(x, size);
    return x;
}

proc echo_array(@array, w, h) {
    for(let x = 0; x < w; x = x + 1) {
        for(let y = 0; y < h; y = y + 1) {
            printf("value at (%d,%d) -> %d\n", x, y, mematl(array, x * w + y));
        }
    }
    return 0;
}

proc init() {
    let w = 12;
    let h = 12;
    let array = allocate_array(w * h * 8);
    srand(time(0));
    for(let x = 0; x < w; x = x + 1) {
        for(let  y = 0; y < h; y = y + 1) {
            let index = (x * w + y);
            memstorel(array, index, rand()%255);
        }
    }
    echo_array(array, w, h);
    free(array);
    return 0;
}
