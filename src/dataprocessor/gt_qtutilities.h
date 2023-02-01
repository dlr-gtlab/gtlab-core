/* GTlab - Gas Turbine laboratory
 * Source File: gt_qtutilities.h
 * copyright 2022 by DLR
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */

#include "gt_typetraits.h"

#include <QObject>

#include <memory>

namespace gt
{


/**
 * @brief Similiar to const_cast but exclusively for casting a container of
 * pointers to a container of const pointers.
 * Cannot be used the other way around.
 *
 * Usage:
 *
 * std::vector<int*> ptrs = ...;
 *
 * std::vector<int const*> const_ptrs = gt::container_const_cast(ptrs);
 *
 * @param contianer Container to cast
 * @return Const container
 */
template<template<class...> class Vec, typename... T>
auto& container_const_cast(Vec<T...>& contianer)
{
    void* ptr = static_cast<void*>(&contianer); // :)
    return *static_cast<Vec<gt::trait::to_const_t<T>...>*>(ptr);
}

/**
 * @brief Overlaod for const l-values
 * @param container Container to cast
 * @return Const container
 */
template<template<class...> class Vec, typename... T>
auto const& container_const_cast(Vec<T...> const& contianer)
{
    void const* ptr = static_cast<void const*>(&contianer); // :)
    return *static_cast<Vec<gt::trait::to_const_t<T>...> const*>(ptr);
}

/**
 * @brief Overlaod for r-values
 * @param container Container to cast
 * @return Const container
 */
template<template<class...> class Vec, typename... T>
auto container_const_cast(Vec<T...>&& contianer)
{
    void* ptr = static_cast<void*>(&contianer); // :)
    return *static_cast<Vec<gt::trait::to_const_t<T>...>*>(ptr);
}

/**
 * @brief Performs a qobject_cast for unique pointers
 *
 * This transfers the ownership of the source pointer to the target
 * pointer
 *
 * Usage:
 *
 * std::unique_ptr<Base> base = ...;
 *
 * std::unique_ptr<Derived> derived = gt::unique_qobject_cast<Derived>(std::move(base));
 *
 * @param src
 * @return
 */
template<typename Derived, typename Base>
std::unique_ptr<Derived>
unique_qobject_cast(std::unique_ptr<Base>&& basePtr) noexcept
{
    auto derivedPtr = std::unique_ptr<Derived>(
        qobject_cast<Derived*>(basePtr.get()));

    if (derivedPtr)
    {
        // transfer ownership
        basePtr.release();
        return derivedPtr;
    }

    return nullptr;
}

/**
 * @brief Transforms the container into a stringlist of its object names
 * @param t Container to transform
 * @return object names in container
 */
template <typename Container,
          typename T = gt::trait::value_t<Container>,
          gt::trait::enable_if_ptr_derived_of_qobject<T> = true>
QStringList objectNames(Container const& t)
{
    QStringList names;
    std::transform(std::begin(t), std::end(t),
                   std::back_inserter(names), [](auto const* t){
        assert(t);
        return t->objectName();
    });
    return names;
}

} // namespace gt
