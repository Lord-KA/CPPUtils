#include <iostream>
#include <vector>
#include <random>
#include "gtest/gtest.h"

#include "vector.hpp"

std::mt19937 rnd(179);

using Test_type = long;

TEST(Manual, Constructors)
{
    g::vector<Test_type> D;
    for (int i = 0; i < 40; ++i)
        D.push_back(rnd());

    g::vector<Test_type> V1, V2(112), V3(1, 12345), V4(D.begin(), D.end());

    std::vector<Test_type> STDV1, STDV2(112), STDV3(1, 12345), STDV4(D.begin(), D.end());

    for (size_t i = 0; i < V1.size(); ++i)
        EXPECT_EQ(V1[i], STDV1[i]);
    for (size_t i = 0; i < V2.size(); ++i)
        EXPECT_EQ(V2[i], STDV2[i]);
    for (size_t i = 0; i < V3.size(); ++i)
        EXPECT_EQ(V3[i], STDV3[i]);
    for (size_t i = 0; i < V4.size(); ++i)
        EXPECT_EQ(V4[i], STDV4[i]);

    EXPECT_EQ(V4, STDV4);
    EXPECT_NE(V1, V4);
    g::vector<Test_type> V6(V4);
    V6[2] = rnd();
    EXPECT_NE(V6, V4);
    g::vector<Test_type> V5(std::move(V4));
    std::vector<Test_type> STDV5(std::move(STDV4));

    for (size_t i = 0; i < V5.size(); ++i)
        EXPECT_EQ(V5[i], STDV5[i]);
}

TEST(Manual, Assign)
{
    g::vector<Test_type> D;
    for (int i = 0; i < 40; ++i)
        D.push_back(rnd());

    g::vector<Test_type> V1, V2, V3;
    std::vector<Test_type> STDV1, STDV2, STDV3;
    V1.assign(D.begin(), D.end());
    STDV1.assign(D.begin(), D.end());
    EXPECT_EQ(V1, STDV1);

    int a = rnd() % 500;
    int b = rnd();
    V2.assign(a, b);
    STDV2.assign(a, b);
    EXPECT_EQ(V2, STDV2);

    V3 = V1;
    STDV3 = STDV1;
    EXPECT_EQ(V1, STDV1);
    EXPECT_EQ(V1, V3);
    EXPECT_EQ(V3, STDV3);

    V2.assign(V1.begin(), V1.end() - 1);
    STDV2.assign(V1.begin(), V1.end() - 1);
    EXPECT_NE(V2, V1);
    EXPECT_EQ(V2, STDV2);

    V3 = std::move(V2);
    STDV3 = std::move(STDV2);
    EXPECT_EQ(V3, STDV3);
}

TEST(Manual, ElementAccess)
{
    g::vector<Test_type> D;
    for (int i = 0; i < 40; ++i)
        D.push_back(rnd());

    g::vector<Test_type> V1, V2, V3;
    std::vector<Test_type> STDV1, STDV2, STDV3;
    V1.assign(D.begin(), D.end());
    STDV1.assign(D.begin(), D.end());
    EXPECT_EQ(V1, STDV1);

    for (size_t i = 0; i < V1.size(); ++i){
        EXPECT_EQ(V1[i], STDV1[i]);
        EXPECT_EQ(V1.at(i), STDV1.at(i));
    }
    EXPECT_EQ(V1.front(), STDV1.front());
    EXPECT_EQ(V1.back(), STDV1.back());

    for (size_t i = 0; i < V1.size(); ++i){
        ++++V1[i];
        ++++STDV1[i];
    }
    for (size_t i = 0; i < V1.size(); ++i){
        EXPECT_EQ(V1[i], STDV1[i]);
        EXPECT_EQ(V1.at(i), STDV1.at(i));
    }

}

TEST(Iterators, Forward)
{
    g::vector<Test_type> D;
    for (int i = 0; i < 40; ++i)
        D.push_back(rnd());

    g::vector<Test_type> V1, V2, V3;
    std::vector<Test_type> STDV1, STDV2, STDV3;
    V1.assign(D.begin(), D.end());
    STDV1.assign(D.begin(), D.end());
    EXPECT_EQ(V1, STDV1);

    auto iter_1 = V1.begin();
    auto iter_2 = STDV1.begin();

    while (iter_1 != V1.end()){
        EXPECT_EQ(*(iter_1++), *(iter_2++));
    }

}

TEST(Iterators, Backward)
{
    g::vector<Test_type> D;
    for (int i = 0; i < 40; ++i)
        D.push_back(rnd());

    g::vector<Test_type> V1, V2, V3;
    std::vector<Test_type> STDV1, STDV2, STDV3;
    V1.assign(D.begin(), D.end());
    STDV1.assign(D.begin(), D.end());
    EXPECT_EQ(V1, STDV1);

    auto iter_1 = V1.end();
    auto iter_2 = STDV1.end();

    while (iter_1 != V1.begin()){
        EXPECT_EQ(*(--iter_1), *(--iter_2));
    }
}


TEST(Iterators, RandomAccess)
{
    g::vector<Test_type> D;
    for (int i = 0; i < 1000 + rnd() % 3000; ++i)
        D.push_back(rnd());

    g::vector<Test_type> V1, V2, V3;
    std::vector<Test_type> STDV1, STDV2, STDV3;
    V1.assign(D.begin(), D.end());
    STDV1.assign(D.begin(), D.end());
    EXPECT_EQ(V1, STDV1);


    for (int i = 0; i < 100; ++i){
        int a = rnd() % (V1.size() / 2 - 1) + 1;
        int b = -(rnd() % (V1.size() / 2 - 1) + 1);
        auto iter = V1.begin();
        iter += a;
        EXPECT_EQ(*(iter), V1[a]);
        auto iter2 = V1.end();
        EXPECT_EQ(*(iter2 - a), V1[V1.size() - a]);
        iter2 -= b;
    }
}


TEST(Manual, Capacity)
{
    g::vector<Test_type> D;
    for (int i = 0; i < 1000 + rnd() % 3000; ++i)
        D.push_back(rnd());

    g::vector<Test_type> V1, V2, V3;
    std::vector<Test_type> STDV1, STDV2, STDV3;
    V1.assign(D.begin(), D.end());
    STDV1.assign(D.begin(), D.end());
    EXPECT_EQ(V1, STDV1);

    EXPECT_EQ(V1.size(), STDV1.size());
    int a = rnd() % 50000;
    V1.reserve(a);
    EXPECT_EQ(V1.capacity(), a);
    V1.shrink_to_fit();
    STDV1.shrink_to_fit();

    EXPECT_EQ(V1, STDV1);
}

TEST(Manual, Resize)
{
    g::vector<Test_type> V1;
    std::vector<Test_type> STDV1;
    for (int i = 0; i < 10 + rnd() % 100; ++i){
        int q = rnd();
        V1.push_back(q);
        STDV1.push_back(q);
    }
    EXPECT_EQ(V1.size(), STDV1.size());
    EXPECT_EQ(V1, STDV1);

    int q = rnd() % 1000;
    V1.resize(200 + q);
    STDV1.resize(200 + q);
    EXPECT_EQ(V1.size(), STDV1.size());
    EXPECT_EQ(V1, STDV1);

    q = rnd() % 200;
    V1.resize(q);
    STDV1.resize(q);
    EXPECT_EQ(V1.size(), STDV1.size());
    EXPECT_EQ(V1, STDV1);
}

TEST(Manual, Modifiers)
{
    g::vector<Test_type> D;
    for (int i = 0; i < 1000 + rnd() % 3000; ++i)
        D.push_back(rnd());

    g::vector<Test_type> V1, V2, V3;
    std::vector<Test_type> STDV1, STDV2, STDV3;
    V1.assign(D.begin(), D.end());
    STDV1.assign(D.begin(), D.end());
    EXPECT_EQ(V1, STDV1);

    V2 = V1;
    STDV2 = STDV1;
    V2.clear();
    STDV2.clear();
    EXPECT_EQ(V2, STDV2);

    int a = rnd() % 200;
    int b = rnd();
    V2 = V1;
    STDV2 = STDV1;

    V2.push_back(b);
    STDV2.push_back(b);

    EXPECT_EQ(V2, STDV2);
    EXPECT_NE(V2, V1);
    V2.pop_back();
    STDV2.pop_back();
    EXPECT_EQ(V2, V1);

    EXPECT_EQ(V2, STDV2);

    V2.resize(V2.size() + a, b);
    STDV2.resize(STDV2.size() + a, b);
    EXPECT_EQ(V2, STDV2);

    V2 = V1;
    STDV2 = STDV1;

    a = rnd() % 600;
    V2.resize(V2.size() - a);
    STDV2.resize(STDV2.size() - a);
    EXPECT_EQ(V2, STDV2);

    V2 = V1;
    STDV2 = STDV1;

    V2[a] = b;
    STDV2[a] = b;

    swap(V1, V2);
    std::swap(STDV1, STDV2);
    EXPECT_EQ(V1, STDV1);
    EXPECT_EQ(V2, STDV2);
    EXPECT_NE(V1, V2);

    V3 = V2;
    V3.push_back(13);
    V2 = V3;
    g::vector V4 = std::move(V3);
    EXPECT_EQ(V2,V4);
}

TEST(Manual, Insert)
{
    g::vector<Test_type> V2;
    std::vector<Test_type> STDV2;
    for (size_t i = 0; i < 10; ++i){
        int q = rnd();
        V2.push_back(q);
        STDV2.push_back(q);
    }
    EXPECT_EQ(V2, STDV2);
    size_t a = 3;
    int b = 3e6;
    int c = 4;
    V2.insert(V2.begin() + a, b);
    STDV2.insert(STDV2.begin() + a, b);
    EXPECT_EQ(V2, STDV2);


    V2.dump(std::cerr);
    V2.insert(V2.begin() + a, c, b);
    STDV2.insert(STDV2.begin() + a, c, b);
    V2.dump(std::cerr);
    EXPECT_EQ(V2, STDV2);


    g::vector<Test_type> V3;
    std::vector<Test_type> STDV3;
    for (size_t i = 0; i < 10; ++i){
        int q = rnd();
        V3.push_back(q);
        STDV3.push_back(q);
    }
    EXPECT_EQ(V3, STDV3);
    EXPECT_EQ(V2, STDV2);
    a = 5;

    V3.insert(V3.begin() + a, V2.begin() + 3, V2.begin() + 5);
    STDV3.insert(STDV3.begin() + a, STDV2.begin() + 3, STDV2.begin() + 5);
    EXPECT_EQ(V3, STDV3);

    g::vector<Test_type> V4;
    std::vector<Test_type> STDV4;
    for (size_t i = 0; i < 10; ++i){
        int q = rnd();
        V4.push_back(q);
        STDV4.push_back(q);
    }
    EXPECT_EQ(V4, STDV4);
    EXPECT_EQ(V3, STDV3);
    a = 7;
    V4.insert(V4.begin() + a, {1, 2, 3, 4, 5});
    STDV4.insert(STDV4.begin() + a, {1, 2, 3, 4, 5});
    EXPECT_EQ(V4, STDV4);
}

TEST(Manual, Erase)
{
    g::vector<Test_type> V2;
    std::vector<Test_type> STDV2;
    for (size_t i = 0; i < 10; ++i){
        int q = rnd();
        V2.push_back(q);
        STDV2.push_back(q);
    }
    EXPECT_EQ(V2, STDV2);
    size_t a = 3;
    size_t c = 5;
    V2.erase(V2.begin() + a);
    STDV2.erase(STDV2.begin() + a);
    EXPECT_EQ(V2, STDV2);

    V2.erase(V2.begin() + a, V2.begin() + c);
    STDV2.erase(STDV2.begin() + a, STDV2.begin() + c);
    EXPECT_EQ(V2, STDV2);
}

TEST(Manual, Emplace)
{
    g::vector<Test_type> V2;
    std::vector<Test_type> STDV2;
    for (size_t i = 0; i < 10; ++i){
        int q = rnd();
        V2.push_back(q);
        STDV2.push_back(q);
    }
    EXPECT_EQ(V2, STDV2);
    size_t a = 3;
    int c = 3e4, d = 7e6;

    V2.emplace(V2.begin() + a, c);
    STDV2.emplace(STDV2.begin() + a, c);
    EXPECT_EQ(V2, STDV2);

    V2.emplace_back(d);
    STDV2.emplace_back(d);
    EXPECT_EQ(V2, STDV2);
}

TEST(Manual, InitList)
{
    g::vector<Test_type> V1({1, 2, 3, 4});
    g::vector<Test_type> V2;
    V2 = {5, 6, 7, 8, 9};
    V2 = V1;
    EXPECT_EQ(V1, V2);
    V1 = V2;
    EXPECT_EQ(V1, V2);
    std::vector<Test_type> STDV2 = {1};

    V2.assign({11, 12, 13, 14, 15, 16});
    STDV2.assign({11, 12, 13, 14, 15, 16});
    EXPECT_EQ(STDV2, V2);

}



int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
