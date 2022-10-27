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

template <typename T>
struct is_generic_pointer :
        std::integral_constant<bool,
               std::is_pointer<T>::value &&
               !std::is_base_of<QObject, std::remove_pointer_t<T>>::value> {};

template <typename T, typename U>
using if_convertible =
        std::enable_if_t<std::is_convertible<T, U>::value, bool>;

template <typename T, typename U>
using if_not_convertible =
        std::enable_if_t<!std::is_convertible<T, U>::value, bool>;

} // namespace trait

} // namespace gt

#endif // GT_TYPETRAITS_H
