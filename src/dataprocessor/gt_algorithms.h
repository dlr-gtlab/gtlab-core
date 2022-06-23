/* GTlab - Gas Turbine laboratory
 * Source File: gt_algorithms.h
 * copyright 2009-2020 by DLR
 *
 *  Created on: 09.03.2022
 *  Author: Martin Siggel (AT-TWK)
 *  Tel.: +49 2203 601 2264
 */


#ifndef GT_ALGORITHMS_H
#define GT_ALGORITHMS_H

#include <algorithm>

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

#endif // GT_ALGORITHMS_H
