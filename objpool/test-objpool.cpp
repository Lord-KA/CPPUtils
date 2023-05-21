#include "objpool.hpp"

#include <cassert>
#include <set>

class Tester {
public:
    Tester(int n)
        : n(n)
    {
        std::cerr << "Tester (" << n <<  ") Ctor called!\n";
    }

    ~Tester()
    {
        assert(n != -0x179);
        std::cerr << "Tester (" << n <<  ") Dtor called!\n";
        n = -0x179;
    }
private:
    int n = -0x179;
};

int main()
{
    const int n = 8;
    g::ObjPool<Tester> pool(3);

    for (int i = 0; i < n + 1; ++i)
    {
        std::cerr << pool.InitAlloc(i) << "\n";
    }
    pool.Free(0);
    pool.Free(0x0000000200000000);

    pool.InitAlloc(-1);

    pool.InitAlloc(-2);

    pool.InitAlloc(-3);
}
