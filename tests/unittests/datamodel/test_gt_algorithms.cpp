/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gt_algorithms.h"

#include <QObject>

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

TEST(TestGtAlgorithms, is_child_of)
{
    QObject root;
    QObject A, B;
    QObject A_1, A_2;
    QObject A_1_1;
    QObject B_1;

    // no parent yet
    EXPECT_FALSE(gt::is_child_of(A, root, gt::get_parent));

    // root
    //  - A
    A.setParent(&root);
    EXPECT_TRUE(gt::is_child_of(A, root, gt::get_parent));
    EXPECT_FALSE(gt::is_child_of(root, A, gt::get_parent));

    // root
    //  - A
    //   - A_1
    //   - A_2
    A_1.setParent(&A);
    A_2.setParent(&A);
    EXPECT_TRUE(gt::is_child_of(A_1, A, gt::get_parent));
    EXPECT_TRUE(gt::is_child_of(A_2, A, gt::get_parent));
    EXPECT_TRUE(gt::is_child_of(A_2, root, gt::get_parent));

    EXPECT_FALSE(gt::is_child_of(B_1, root, gt::get_parent));

    B.setParent(&root);
    EXPECT_TRUE(gt::is_child_of(B, root, gt::get_parent));
}

TEST(TestGtAlgorithms, find_lowest_ancestor)
{
    QObject root;
    QObject A, B;
    QObject A_1, A_2;
    QObject A_1_1;
    QObject B_1;
    QObject B_1_1;
    QObject B_1_2;

    A.setParent(&root);
    A_1.setParent(&A);
    A_1_1.setParent(&A_1);
    A_2.setParent(&A);

    B.setParent(&root);
    B_1.setParent(&B);
    B_1_1.setParent(&B_1);
    B_1_2.setParent(&B_1);

    std::vector<QObject*> test_0 = {&root};
    EXPECT_EQ(gt::find_lowest_ancestor(test_0, gt::get_parent), nullptr);

    std::vector<QObject*> test_1 = {&A_1_1};
    EXPECT_EQ(gt::find_lowest_ancestor(test_1, gt::get_parent), &A_1);

    std::vector<QObject*> test_2 = {&A_1_1, &A_1, &A_2};
    EXPECT_EQ(gt::find_lowest_ancestor(test_2, gt::get_parent), &A);

    std::vector<QObject*> test_3 = {&A_1_1, &B_1_2, &A_1, &A_2};
    EXPECT_EQ(gt::find_lowest_ancestor(test_3, gt::get_parent), &root);

    std::vector<QObject*> test_4 = {&B_1_1, &B_1_2};
    EXPECT_EQ(gt::find_lowest_ancestor(test_4, gt::get_parent), &B_1);
}
