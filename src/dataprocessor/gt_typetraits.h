/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
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

/// Enabled for pointer types not derived of QObject
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

/// Enabled if T is derived of Base
template <typename Base, typename T>
using enable_if_base_of =
        std::enable_if_t<std::is_base_of<Base, T>::value, bool>;

/// Enabled if T is a pointer and is derived of Base
template <typename Base, typename T>
using enable_if_ptr_base_of =
        std::enable_if_t<
            std::is_pointer<T>::value &&
            std::is_base_of<Base, std::remove_pointer_t<T>>::value, bool>;

/// Enabled if T is derived of QObject
template < typename T>
using enable_if_derived_of_qobject = enable_if_base_of<QObject, T>;

/// Enabled if T is derived of QObject
template < typename T>
using enable_if_ptr_derived_of_qobject = enable_if_ptr_base_of<QObject, T>;

} // namespace trait

} // namespace gt

#endif // GT_TYPETRAITS_H
