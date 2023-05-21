#include "mempool.hpp"
#include <cassert>
#include <cstddef>
#include <stdio.h>

int main()
{
    size_t cap = 3;
    g::MemPool pool(sizeof(size_t), cap);


    size_t *ptr1 = NULL;
    size_t *ptr2 = NULL;
    for (size_t i = 0; i < cap + 12; ++i) {
        size_t *data = (size_t*)pool.allocate(sizeof(size_t));
        *data = 179 - i;
        fprintf(stderr, "allocated %p\n", data);

        if (i == 3)
            ptr1 = data;

        if (i == 11)
            ptr2 = data;
        assert(*data == 179 - i);
    }

    pool.deallocate(ptr1);
    pool.deallocate(ptr2);

    for (size_t i = 0; i < 29; ++i) {
        size_t *data = (size_t*)pool.allocate(sizeof(size_t));
        *data = 179 - i;
        fprintf(stderr, "allocated %p\n", data);

        if (i == 17)
            ptr1 = data;

        if (i == 26)
            ptr2 = data;
        assert(*data == 179 - i);
    }

    pool.deallocate(ptr1);
    pool.deallocate(ptr2);
}
