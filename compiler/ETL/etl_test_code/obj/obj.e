#include<libetl/etl.e>

proc program() {
puts("Hello, World");
let x = 25;
let y = 25;
let z = x * y / 100;
printf("%d * %d / 100 = %d\n", x, y, z);
return 0;
}

