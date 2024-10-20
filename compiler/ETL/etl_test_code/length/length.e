#include<libetl/etl.e>

proc testLen($what) {
    let n_s = malloc (strlen("hey: ") + strlen(what) + 1 + 255);
    sprintf(n_s, "hey: %s len: %d", what, strlen(what));
    puts(string(n_s));
    let length = strlen(string(n_s));
    free(n_s);
    return length;
}

proc init() {
    printf("THIS IS A TEST Len: %d\n", testLen("ABC"));
    return 0;
}