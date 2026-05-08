program float_stress;
var
x, y, z: real;

procedure test;
var
    a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t : real;
    sum, tmp : real;

begin
    a := 1.1;  b := 2.2;  c := 3.3;  d := 4.4;
    e := 5.5;  f := 6.6;  g := 7.7;  h := 8.8;
    i := 9.9;  j := 10.10; k := 11.11; l := 12.12;
    m := 13.13; n := 14.14; o := 15.15; p := 16.16;
    q := 17.17; r := 18.18; s := 19.19; t := 20.20;

    sum := 0.0;

    { allocate lots of float temporaries via complex expressions }
    tmp := a*b + c/d - e*f + g/h - i*j + k/l - m/n + o/p - q/r + s/t;
    sum := sum + tmp;

    tmp := (a + b + c + d + e) * (f + g + h + i + j);
    sum := sum + tmp;

    tmp := (k*l - m*n) / (o + p + q) + (r*s) * (t + a);
    sum := sum + tmp;

    { big linear combination to stress registers/reuse }
    sum := sum + a + b + c + d + e + f + g + h + i + j
               + k + l + m + n + o + p + q + r + s + t;

    writeln(sum);
end;
begin
test;
x := 0;
y := 1;
z := 2;
x := y * z;
writeln(x);
end.
