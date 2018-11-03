#include <iostream>
#include <string>
#include <typeinfo>

template<typename T>
auto getMax() {
    return [](T a, T b){
               return a > b ? a : b;
           };
}
int main () {
    auto a = getMax<int>();
    auto b = a(1,3 );
    std::cout << b << std::endl;
}
