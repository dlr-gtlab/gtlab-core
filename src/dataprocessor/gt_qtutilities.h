/* GTlab - Gas Turbine laboratory
 * Source File: gt_qtutilities.h
 * copyright 2022 by DLR
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */

#ifndef GT_QTUTILITIES_H
#define GT_QTUTILITIES_H

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

namespace detail
{

template<typename ObjectList, typename GetNameFunc>
inline QString
makeUniqueNameImpl(const QString& name,
                         const ObjectList& objs,
                         const GetNameFunc& getName,
                         QString initName = {},
                         int iteration = 2)
{
    auto iter = std::find_if(std::begin(objs), std::end(objs),
                             [&](const auto& o) {
        return name == getName(o);
    });

    if (iter == std::end(objs)) return name;

    if (initName.isEmpty()) initName = name;

    QString newName = initName +
                      QStringLiteral("[") +
                      QString::number(iteration) +
                      QStringLiteral("]");

    return makeUniqueNameImpl(
                newName, objs, getName, std::move(initName), iteration + 1);
}

} // namespace detail

/**
 * @brief Returns a unique name given a list of objects names
 *
 * @param name The base name. If e.g. "aa" already exists, "aa[1]" is returned
 * @param objs List of objects to query from
 * @param func A function to get the name from an object
 * @return A unique name
 */
template<typename ObjectList, typename GetNameFunc>
inline QString
makeUniqueName(const QString& name,
                     const ObjectList& objs,
                     const GetNameFunc& getName)
{
    if (name.isEmpty()) return {};

    return detail::makeUniqueNameImpl(name, objs, getName);
}

/**
 * @brief Returns a unique name given a list of names
 *
 * @param name The base name. If e.g. "aa" already exists, "aa[1]" is returned
 * @param names List of names to query from
 * @return A unique name
 */
template<typename StringList,
         typename T = trait::value_t<StringList>,
         trait::enable_if_convertible<T, QString> = true>
inline QString
makeUniqueName(const QString& name, const StringList& names)
{
    if (name.isEmpty()) return {};

    return detail::makeUniqueNameImpl(
                name, names, [](const auto& listItem) -> QString {
        return listItem;
    });
}

/**
 * @brief Creates an unique object name based on given initial string and
 * a parent object reference.
 *
 * @param Initial object name
 * @param Parent obje
 * @return Unique object name
 */
inline QString
makeUniqueName(const QString& name, const QObject& parent)
{
    if (name.isEmpty()) return {};

    auto const childs = parent.findChildren<const QObject*>(
                            QString{}, Qt::FindDirectChildrenOnly);

    return detail::makeUniqueNameImpl(name, childs, [](const QObject* o){
        return o->objectName();
    });
}

} // namespace gt

#endif // GT_QTUTILITIES_H
