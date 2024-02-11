#include <iostream>
#include "function.hpp"

int sum(int a, int b)
{
    return a + b;
}

int pow(int a, int b)
{
    int res = 1;
    for (int i = 0; i < b; ++i) {
        res *= a;
    }
    return res;
}

int main()
{
    g::function<int(int, int)> f = sum;

    std::cout << f(2, 3) << std::endl;

    f = pow;

    std::cout << f(2, 3) << std::endl;
}
