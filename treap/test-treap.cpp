#include "treap.hpp"
#include <vector>
#include "gtest/gtest.h"

static std::mt19937 rnd(179);

template<typename T>
void IteratorsTest(){
    g::treap<int, T> T1;
    for (int i = 0; i < 500 + rnd() % 1500; ++i){
        T1.insert(static_cast<int>(rnd()), static_cast<T>(rnd()));
    }
    auto iter = T1.begin();
    for (size_t i = 0; i < T1.size() - 1; ++i){
        EXPECT_EQ((*iter).second, T1[i]);
        ++iter;
    }
    for (size_t i = T1.size() - 1; i > 0; --i){
        EXPECT_EQ((*iter).second, T1[i]);
        --iter;
    }

    T1.erase((*iter).first);
    for (int i = 0; i < 100; ++i){
        iter = T1.begin() + rnd() % 400;
        T1.erase((*iter).first);
    }
    T *n = T1.insert(static_cast<int>(rnd()));

    *n = 102;

    iter = T1.begin();
    for (size_t i = 0; i < T1.size() - 1; ++i){
        EXPECT_EQ((*iter).second, T1[i]);
        ++iter;
    }
    *n = 102;
    iter = T1.begin();
    for (size_t i = 0; i < T1.size(); ++i){
        ++T1[i];
        EXPECT_EQ((*iter).second, T1[i]);
        ++iter;
    }
}


TEST(Basics, Iterators)
{
    for (int u = 0; u < 1; ++u){

        IteratorsTest<int>();
        IteratorsTest<long>();
        IteratorsTest<unsigned long long>();
        IteratorsTest<short>();
        IteratorsTest<char>();
    }
}

TEST(Basics, InsertEraseFind)
{
    for (int p = 0; p < 5; ++p){
    g::treap<int, int> T1;
    std::vector<int> V1;
    for (int i = 0; i < 500 + rnd() % 1500; ++i){
        int a = rnd();
        T1.insert(a, a);
        V1.push_back(a);
    }
    std::sort(V1.begin(), V1.end());
    for (int i = 0; i < T1.size(); ++i)
        EXPECT_EQ(T1[i], V1[i]);

    for (int i = 0; i < 100; ++i){
        int a = rnd() % 400;
        auto iter = T1.begin() + a;
        T1.erase((*iter).first);
        V1.erase(V1.begin() + a);
    }

    for (int i = 0; i < T1.size(); ++i)
        EXPECT_EQ(T1[i], V1[i]);

    //continue;

    for (int i = 0; i < 100; ++i){
        int a = rnd() % 300, b = rnd();
        int *q = T1.find((*(T1.begin() + a)).first);
        *q = b;
        V1[a] = b;
    }
    for (int i = 0; i < T1.size(); ++i)
        EXPECT_EQ(T1[i], V1[i]);
    }
}

TEST(Basics, CopyMove)
{
    for (int u = 0; u < 6; ++u){
    g::treap<int, int> T1;
    std::vector<int> V1;
    for (int i = 0; i < 500 + rnd() % 1500; ++i){
        int a = rnd();
        T1.insert(a, a);
        V1.push_back(a);
    }
    std::sort(V1.begin(), V1.end());
    for (int i = 0; i < T1.size(); ++i)
        EXPECT_EQ(T1[i], V1[i]);


    g::treap<int, int> T2(T1), T3, T4, T5(std::move(T1));
    T3 = T2;
    for (int i = 0; i < T2.size(); ++i)
        EXPECT_EQ(T2[i], V1[i]);
    for (int i = 0; i < T3.size(); ++i)
        EXPECT_EQ(T3[i], V1[i]);
    for (int i = 0; i < T5.size(); ++i)
        EXPECT_EQ(T5[i], V1[i]);

    T4 = std::move(T5);
    for (int i = 0; i < T4.size(); ++i)
        EXPECT_EQ(T4[i], V1[i]);
    }
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
