/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_algorithms.h
 *
 *  Created on: 09.03.2022
 *  Author: Martin Siggel (AT-TWK)
 *  Tel.: +49 2203 601 2264
 */


#ifndef GT_ALGORITHMS_H
#define GT_ALGORITHMS_H

#include <algorithm>
#include <map>
#include <queue>

#include <cassert>

namespace gt
{

template <typename QMapIterator, typename Func>
void for_each_key(QMapIterator begin, QMapIterator end, Func func)
{
    for (auto iter = begin; iter != end; ++iter)
    {
        func(iter.key());
    }
}

template <typename QMapType, typename Func>
void for_each_key(QMapType map, Func func)
{
    return for_each_key(map.begin(), map.end(), func);
}

template <typename QMapIterator, typename Func>
bool all_of_keys(QMapIterator begin, QMapIterator end, Func func)
{
    auto allTrue = true;
    for (auto iter = begin; iter != end; ++iter)
    {
        allTrue = allTrue && func(iter.key());
    }

    return allTrue;
}

template <typename QMapType, typename Func>
bool all_of_keys(QMapType map, Func func)
{
    return all_of_keys(map.begin(), map.end(), func);
}

template <typename QMapIterator, typename Func>
bool any_of_keys(QMapIterator begin, QMapIterator end, Func func)
{
    for (auto iter = begin; iter != end; ++iter)
    {
        if (func(iter.key())) return true;
    }

    return false;
}

template <typename QMapType, typename Func>
bool any_of_keys(QMapType map, Func func)
{
    return any_of_keys(map.begin(), map.end(), func);
}
/**
 * @brief Returns the iterator to the ith element in the container
 */
template <typename ContainerType>
typename ContainerType::iterator
ith_iter(ContainerType& container, size_t i)
{
    auto iter = container.begin();
    std::advance(iter, i);

    return iter;
}

template <typename ContainerType>
typename ContainerType::const_iterator
ith_iter(const ContainerType& container, size_t i)
{
    auto iter = container.begin();
    std::advance(iter, i);

    return iter;
}

/**
 * @brief Performs a topological sort of the given adjacency matrix
 *        and returns the keys of the map in the correct sorted order.
 *
 *        This is Kahns algorithm!
 *        (https://en.wikipedia.org/wiki/Topological_sorting)
 *
 * @param adjList An adjacency list to sort
 * @return
 */
template <typename Type, typename VectorType=std::vector<Type>>
VectorType
topo_sort(const std::map<Type, VectorType>& adjList)
{
    // compute in-degree (number of incoming edges) of every node
    std::map<Type, size_t> in_degrees;
    for (const auto& entry : adjList) {
        in_degrees[entry.first] = 0;
    }

    for (const auto& entry: adjList) {
        const auto& childs = entry.second;
        for (const Type& child : childs) {
            in_degrees[child] += 1;
        }
    }

    // queue of items with in_degree == 0
    std::queue<Type> s;

    // find root items
    for (const auto& entry : adjList) {
        const auto& key = entry.first;
        if (in_degrees[key] == 0) {
            s.push(key);
            in_degrees.erase(key);
        }
    };

    VectorType sorted;
    while (!s.empty()) {
        auto current = s.front();
        sorted.push_back(current);
        s.pop();

        auto childsIt = adjList.find(current);
        assert(childsIt != adjList.end());

        for (const Type& child : childsIt->second) {
            auto iter = adjList.find(child);
            if (iter == adjList.end())
            {
                // the child is not in the list of nodes, illegal graph
                continue;
            }

            if (--in_degrees[child] == 0) {
                s.push(child);
                in_degrees.erase(child);
            }
        }
    }

    return sorted;
}

static const auto get_parent = [](auto* p) { return p->parent(); };
static const auto get_parent_object = [](auto* p) { return p->parentObject(); };

/**
 * @brief Returns true if object is a (sub) child of target
 * @param object Object to query
 * @param target Target parent
 * @param getParent Fucntor to access parent object
 * @tparam T object type
 * @tparam GetParentFunctor Parent functor
 * @return Is object child of target
 */
template <typename T,
          typename U = T, // U might be a base class of T
          typename GetParentFunctor>
inline bool
is_child_of(T& object, U& target, GetParentFunctor const& getParent)
{
    U* parent = &object;

    do // traverse hierarchy up till target was found
    {
        parent = getParent(parent);
    }
    while (parent && parent != &target);

    return parent;
}

/**
 * @brief Finds the lowest (deepest) object that has all other objects as
 * descendants (childs). Inspired by LCA algorithms. Returns null if no common
 * parent was found.
 * @param objects Obects to querry
 * @tparam Container Container type
 * @tparam T Value type of container
 * @tparam GetParentFunctor Parent functor
 * @return Lowest object that has all other objects as children
 */
template <typename Container,
          typename GetParentFunctor>
inline auto
find_lowest_ancestor(Container const& objects,
                     GetParentFunctor const& getParent)
{
    auto beginIter = std::begin(objects);
    auto endIter = std::end(objects);

    // retrieve return type
    using T = decltype(getParent(*beginIter));

    if (beginIter == endIter) return T{};

    // access first parent object
    auto parent = getParent(*beginIter);

    // we dont need to check the first object
    std::advance(beginIter, 1);

    while (parent)
    {
        // find first object that does not have the same parent
        auto iter = std::find_if(beginIter, endIter, [=](auto o){
            assert(o); assert(parent);
            return !is_child_of(*o, *parent, getParent);
        });

        // we are finished if we have iterated over all objects
        if (iter == endIter) break;

        // move up a level
        parent = getParent(parent);
        // we only need to search from "iter" onwards
        beginIter = iter;
    }

    return parent;
}

} // namespace gt

#endif // GT_ALGORITHMS_H
