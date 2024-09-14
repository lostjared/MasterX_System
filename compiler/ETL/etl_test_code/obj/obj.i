# 0 "obj.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "obj.e"
# 1 "/usr/local/include/libetl/etl.e" 1 3




# 4 "/usr/local/include/libetl/etl.e" 3
define scan_integer();
define len($value);
define $str(value);
define exit(code);
define puts($output);
define srand(seed);
define time(value);
define strlen($value);
define strcmp($value1, $value2);
define at($value, index);
# 2 "obj.e" 2


# 3 "obj.e"
proc program() {
puts("Hello, World");
let x = 25;
let y = 25;
let z = x * y / 100;
printf("%d * %d / 100 = %d\n", x, y, z);
return 0;
}
