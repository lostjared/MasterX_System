# 0 "mem.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "mem.e"
# 1 "/usr/local/include/libetl/etl.e" 1 3



# 3 "/usr/local/include/libetl/etl.e" 3
define scan_integer();
define len($value);
define $str(value);
define exit(code);
define puts($output);
define srand(seed);
define rand();
define time(value);
define strlen($value);
define strcmp($value1, $value2);
define at($value, index);
define @malloc(bytes);
define @calloc(bytes, size);
define free(@memory);
define mematl(@memory, index);
define mematb(@memory, index);
define memclr(@memory, size);
define memstorel(@memory, index, value);
define memstoreb(@memory, index, value);
# 2 "mem.e" 2


# 3 "mem.e"
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
        for(let y = 0; y < h; y = y + 1) {
            let index = (x * w + y);
            memstorel(array, index, rand()%255);
        }
    }
    echo_array(array, w, h);
    free(array);
    return 0;
}
