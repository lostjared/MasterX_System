
#include<iostream>

extern "C" long hello() {
    std::cout << "Hello, World frOm C++!\n";
    return 0;
}