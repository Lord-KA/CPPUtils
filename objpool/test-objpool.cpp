#include "objpool.hpp"

#include <gtest/gtest.h>
#include <cassert>
#include <set>
#include <random>
#include <iostream>

typedef size_t GOBJPOOL_TYPE;

std::mt19937 rnd(179);

TEST(manual, basic)
{
    g::ObjPool<GOBJPOOL_TYPE> pool;

	size_t id_1 = pool.alloc();

	auto &data_1 = pool.get(id_1);
	data_1 = 179;

	size_t id_2 = pool.alloc();
	auto &data_2 = pool.get(id_2);
	data_2 = 17900;

	EXPECT_EQ(pool.get(id_1), 179);
	EXPECT_EQ(pool.get(id_2), 17900);

	pool.free(id_1);

	EXPECT_EQ(pool.get(id_2), 17900);

	pool.free(id_2);
}

static const size_t test_shift = 179;

TEST(Auto, Stability)
{
	g::ObjPool<GOBJPOOL_TYPE> pool;

    std::vector<GOBJPOOL_TYPE> crossCheck;

    size_t id = 0;
    for (size_t i = 0; i < 30000; ++i) {
        if (rnd() % 3 != 1) {
			id = pool.alloc();
            crossCheck.push_back(id);

            GOBJPOOL_TYPE &data = pool.get(id);
            data = id + test_shift;
        }
        else {
            size_t pos = rnd() % crossCheck.size();
            if (crossCheck.size() > 0) {
				pool.free(crossCheck[pos]);
                crossCheck.erase(crossCheck.begin() + pos);
            }
        }
        if (crossCheck.size() > 0) {
            size_t pos = rnd() % crossCheck.size();

            GOBJPOOL_TYPE &data = pool.get(crossCheck[pos]);
            EXPECT_EQ(data, crossCheck[pos] + test_shift);
        }
    }
    for (auto elem : crossCheck)
		pool.free(elem);
}

TEST(Auto, MoveCtor)
{
	g::ObjPool<GOBJPOOL_TYPE> first;
	size_t last_id;
	for (size_t i = 0; i < 1000; ++i) {
		last_id = first.alloc();
		first.get(last_id) = 0x179;
	}

	g::ObjPool<GOBJPOOL_TYPE> second(std::move(first));

	ASSERT_EQ(second.get(last_id), 0x179);

	// Nothing should leak after the test is finished.
}

TEST(Auto, MoveAssignment)
{
	g::ObjPool<GOBJPOOL_TYPE> first;
	size_t last_id;
	for (size_t i = 0; i < 1000; ++i) {
		last_id = first.alloc();
		first.get(last_id) = 0x179;
	}

	g::ObjPool<GOBJPOOL_TYPE> second;
	for (size_t i = 0; i < 5000; ++i) {
		size_t id = first.alloc();
		first.get(id) = 0x548;
	}


	second = std::move(first);
	ASSERT_EQ(second.get(last_id), 0x179);

	// Nothing should leak after the test is finished.
}

int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
