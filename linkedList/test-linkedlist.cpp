#include "linkedlist.hpp"
#include <vector>
#include <random>
#include "gtest/gtest.h"

std::mt19937 rnd(179);

TEST(Basics, insertOnTips) {
    for (int k = 0; k < 1000; ++k){
        g::list<int> L1;
        std::vector<int> V1;
        for (int i = 0; i < rnd() % 100 + 10; ++i){
            int a = rnd();
            L1.insert(a);
            V1.insert(V1.begin(), a);
        }
        EXPECT_EQ(L1, V1);

        g::list<int> L2;
        std::vector<int> V2;
        for (int i = 0; i < rnd() % 100 + 10; ++i){
            int a = rnd();
            L2.insert(i, a);
            V2.insert(V2.begin() + i, a);
        }
        EXPECT_EQ(L2, V2);
    }
}
TEST(Basics, insertAndEraseOnTips){
     for (int k = 0; k < 1000; ++k){
        g::list<int> L1;
        std::vector<int> V1;
        size_t i;
        for (i = 0; i < rnd() % 100 + 25; ++i){
            int a = rnd();
            L1.insert(a);
            V1.insert(V1.begin(), a);
        }
        EXPECT_EQ(L1, V1);
        size_t end = i;
        for (; i < end + rnd() % 100 + 25; ++i){
            int a = rnd();
            L1.insert(i, a);
            V1.insert(V1.begin() + i, a);
        }
        EXPECT_EQ(L1, V1);

        for (int j = 0; j < 10; ++j){
            int a = rnd() % 10 + 5;
            L1.erase(a);
            V1.erase(V1.begin() + a);
        }
        L1.erase(0);
        V1.erase(V1.begin());
        EXPECT_EQ(L1, V1);
    }
}

TEST(Basics, operators){
     for (int k = 0; k < 1000; ++k){
        g::list<int> L1;
        std::vector<int> V1;
        size_t i;
        for (i = 0; i < rnd() % 100 + 25; ++i){
            int a = rnd();
            L1.insert(a);
            V1.insert(V1.begin(), a);
        }
        EXPECT_EQ(L1, V1);
        size_t end = i;
        for (; i < end + rnd() % 100 + 25; ++i){
            int a = rnd();
            L1.insert(i, a);
            V1.insert(V1.begin() + i, a);
        }
        EXPECT_EQ(L1, V1);

        for (size_t j = 0; j < L1.size(); ++j)
            EXPECT_EQ(L1[j], V1[j]);

        for (size_t j = 0; j < L1.size(); ++j){
            ++L1[j];
            ++V1[j];
            EXPECT_EQ(L1[j], V1[j]);
        }

        EXPECT_TRUE(L1 == V1);
        EXPECT_FALSE(L1 != V1);
    }
}

TEST(Basics, copyAndMove){
    for (int k = 0; k < 1000; ++k){
        g::list<int> L1;
        std::vector<int> V1;
        size_t i;
        for (i = 0; i < rnd() % 1000 + 25; ++i){
            int a = rnd();
            L1.insert(a);
            V1.insert(V1.begin(), a);
        }
        EXPECT_EQ(L1, V1);
        size_t end = i;
        for (; i < end + rnd() % 1000 + 25; ++i){
            int a = rnd();
            L1.insert(i, a);
            V1.insert(V1.begin() + i, a);
        }
        EXPECT_EQ(L1, V1);
        g::list<int> L2(L1), L3, L5;
        g::list<int> L4(std::move(L1));
        L3 = L1;
        EXPECT_EQ(L2, V1);
        L4.insert(10, rnd());
        L4.insert(10, rnd());
        L3 = L4;
        L5 = L4;
        L5[10] = 10;
        EXPECT_NE(L4, L2);
        EXPECT_NE(L4, L5);
   }
}


int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
