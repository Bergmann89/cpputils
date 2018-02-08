#include <list>
#include <vector>
#include <gtest/gtest.h>

namespace transform_iterator_tests
{
    #include "../src/cpputils/TransformIterator.h"
}

using namespace transform_iterator_tests;
using namespace transform_iterator_tests::utl;

struct op_add_10
{
    inline int operator()(int& i) const
        { return i + 10; }

    inline bool operator==(const op_add_10& other) const
        { return true; }

    inline bool operator!=(const op_add_10& other) const
        { return false; }
};

TEST(TransformIteratorTests, bidirectional_inc)
{
    std::list<int> l { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto it    = makeTransformIterator(l.begin(), op_add_10());
    auto itEnd = makeTransformIterator(l.end(),   op_add_10());
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(11, *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(12, *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(13, *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(14, *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(15, *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(16, *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(17, *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(18, *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(19, *it++);
    ASSERT_EQ(itEnd, it);
}

TEST(TransformIteratorTests, bidirectional_dec)
{
    std::list<int> l { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto itBeg = makeTransformIterator(l.begin(), op_add_10());
    auto it    = makeTransformIterator(l.end(),   op_add_10());
    --it;
    ASSERT_NE(itBeg, it);
    EXPECT_EQ(19, *it--);
    ASSERT_NE(itBeg, it);
    EXPECT_EQ(18, *it--);
    ASSERT_NE(itBeg, it);
    EXPECT_EQ(17, *it--);
    ASSERT_NE(itBeg, it);
    EXPECT_EQ(16, *it--);
    ASSERT_NE(itBeg, it);
    EXPECT_EQ(15, *it--);
    ASSERT_NE(itBeg, it);
    EXPECT_EQ(14, *it--);
    ASSERT_NE(itBeg, it);
    EXPECT_EQ(13, *it--);
    ASSERT_NE(itBeg, it);
    EXPECT_EQ(12, *it--);
    ASSERT_EQ(itBeg, it);
    EXPECT_EQ(11, *it--);
}

TEST(TransformIteratorTests, random_access)
{
    std::vector<int> l { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto it    = makeTransformIterator(l.begin(), op_add_10());
    auto itEnd = makeTransformIterator(l.end(),   op_add_10());
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(9, itEnd - it);
    EXPECT_EQ(11, it[0]);
    EXPECT_EQ(12, it[1]);
    EXPECT_EQ(13, it[2]);
    EXPECT_EQ(14, it[3]);
    EXPECT_EQ(15, it[4]);
    EXPECT_EQ(16, it[5]);
    EXPECT_EQ(17, it[6]);
    EXPECT_EQ(18, it[7]);
    EXPECT_EQ(19, it[8]);
}

TEST(TransformIteratorTests, lambda)
{
    std::vector<int> l { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto lambda = [](int& i) { return &i; };

    auto it    = makeTransformIterator(l.begin(), lambda);
    auto itEnd = makeTransformIterator(l.end(),   lambda);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(&l.at(0), *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(&l.at(1), *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(&l.at(2), *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(&l.at(3), *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(&l.at(4), *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(&l.at(5), *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(&l.at(6), *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(&l.at(7), *it++);
    ASSERT_NE(itEnd, it);
    EXPECT_EQ(&l.at(8), *it++);
    ASSERT_EQ(itEnd, it);
}