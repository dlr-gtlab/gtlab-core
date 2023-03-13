#include "gt_algorithms.h"

#include <gtest/gtest.h>
#include <map>

// Using example from https://en.wikipedia.org/wiki/Topological_sorting
/*
 * 5    7  3
 * |  / | /|
 * | /  |/ |
 * 11-\ 8  |
 * | \ \|  |
 * |  \ |\ |
 * 2   9  10
 */
TEST(TestGtAlgorithms, topoSort)
{
    // create adjacency matrix

    std::map<int, std::vector<int>> matrix;

    // randomly creating entry order
    matrix.insert(std::make_pair(11, std::vector<int>{2, 9, 10}));
    matrix.insert(std::make_pair(5, std::vector<int>{11}));
    matrix.insert(std::make_pair(8, std::vector<int>{9}));
    matrix.insert(std::make_pair(2, std::vector<int>{}));
    matrix.insert(std::make_pair(9, std::vector<int>{}));
    matrix.insert(std::make_pair(7, std::vector<int>{11, 8}));
    matrix.insert(std::make_pair(10, std::vector<int>{}));
    matrix.insert(std::make_pair(3, std::vector<int>{8, 10}));

    auto result = gt::topo_sort(matrix);

    // return iterator to number in result
    auto findNumber = [&result](int n)
    {
        return std::find(result.begin(), result.end(), n);
    };

    ASSERT_EQ(8u, result.size());

    // check that all nodes are in result
    ASSERT_TRUE(findNumber(11) != result.end());
    ASSERT_TRUE(findNumber(5) != result.end());
    ASSERT_TRUE(findNumber(8) != result.end());
    ASSERT_TRUE(findNumber(2) != result.end());
    ASSERT_TRUE(findNumber(9) != result.end());
    ASSERT_TRUE(findNumber(7) != result.end());
    ASSERT_TRUE(findNumber(10) != result.end());
    ASSERT_TRUE(findNumber(3) != result.end());

    // check order
    ASSERT_TRUE(findNumber(5) < findNumber(11));
    ASSERT_TRUE(findNumber(5) < findNumber(2));
    ASSERT_TRUE(findNumber(5) < findNumber(9));
    ASSERT_TRUE(findNumber(5) < findNumber(10));

    ASSERT_TRUE(findNumber(7) < findNumber(11));
    ASSERT_TRUE(findNumber(7) < findNumber(8));
    ASSERT_TRUE(findNumber(7) < findNumber(2));
    ASSERT_TRUE(findNumber(7) < findNumber(9));
    ASSERT_TRUE(findNumber(7) < findNumber(10));

    ASSERT_TRUE(findNumber(3) < findNumber(8));
    ASSERT_TRUE(findNumber(3) < findNumber(10));
    ASSERT_TRUE(findNumber(3) < findNumber(9));

    ASSERT_TRUE(findNumber(11) < findNumber(2));
    ASSERT_TRUE(findNumber(11) < findNumber(9));
    ASSERT_TRUE(findNumber(11) < findNumber(10));

    ASSERT_TRUE(findNumber(8) < findNumber(9));
}
