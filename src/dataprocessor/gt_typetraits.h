/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 20.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GT_TYPETRAITS_H
#define GT_TYPETRAITS_H

#include <type_traits>
#include <QObject>

namespace gt
{
namespace trait
{

/// struct to transform T to T const
template <typename T>
struct to_const
{
    using type = T const;
};

/// Alias. Converts "T" to "T const", "T*" to "T const*" etc.
template <typename T>
using to_const_t = typename to_const<T>::type;

/// specialization for T* to T const*
template <typename T>
struct to_const<T*>
{
    using type = T const*;
};

/// specialization for std::allocator<T> (need for e.g. std::vector)
template <typename T>
struct to_const<std::allocator<T>>
{
    using type = std::allocator<to_const_t<T>>;
};

/// specialization for reference_wrapper
template <typename T>
struct to_const<std::reference_wrapper<T>>
{
    using type = std::reference_wrapper<to_const_t<T>>;
};

/// Access value_type of Container
template <typename Container>
using value_t = typename std::decay_t<Container>::value_type;

/// Transforms T* to T const*
template <typename T>
using const_ptr = typename std::remove_pointer_t<T> const*;

/// True for pointer types not derived of QObject
template <typename T>
struct is_generic_pointer :
        std::integral_constant<bool,
               std::is_pointer<T>::value &&
               !std::is_base_of<QObject, std::remove_pointer_t<T>>::value> {};

/// Enabled if T is convertible to U
template <typename T, typename U>
using enable_if_convertible =
        std::enable_if_t<std::is_convertible<T, U>::value, bool>;

/// Enabled if T is not convertible to U
template <typename T, typename U>
using enable_if_not_convertible =
        std::enable_if_t<!std::is_convertible<T, U>::value, bool>;

// used to enable template if types are equal
template<typename T, typename U>
using enable_if_types_equal =
        std::enable_if_t<
            std::is_same<T, U>::value, bool>;

/// Enabled if T is derived of Base
template <typename Base, typename T>
using enable_if_base_of =
        std::enable_if_t<std::is_base_of<Base, T>::value, bool>;

/// Enabled if T is derived of QObject
template < typename T>
using enable_if_derived_of_qobject = enable_if_base_of<QObject, T>;

/// Enabled if T is a pointer and is derived of Base
template <typename Base, typename T>
using enable_if_ptr_base_of =
        std::enable_if_t<
            std::is_pointer<T>::value &&
            std::is_base_of<Base, std::remove_pointer_t<T>>::value, bool>;

/// Enabled if T is a ptr and is derived of QObject
template < typename T>
using enable_if_ptr_derived_of_qobject = enable_if_ptr_base_of<QObject, T>;

/// Enabled if T is a const ptr
template <typename T>
using enable_if_ptr_const =
        std::enable_if_t<
            std::is_pointer<T>::value &&
            std::is_same<std::remove_const_t<std::remove_pointer_t<T>> const*, T>::value, bool>;

/// Enabled if T is a non-const ptr
template <typename T>
using enable_if_ptr_not_const =
        std::enable_if_t<
            std::is_pointer<T>::value &&
            std::is_same<std::remove_const_t<std::remove_pointer_t<T>>*, T>::value, bool>;

} // namespace trait

} // namespace gt

#endif // GT_TYPETRAITS_H
