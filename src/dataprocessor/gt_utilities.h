/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 23.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GT_UTILITIES_H
#define GT_UTILITIES_H

#include "gt_finally.h" // to keep API compatibility

#include <utility>
#include <limits>
#include <algorithm>

namespace gt
{

/**
 * @brief Quotes a string type using a custom prefix and suffix. By default
 * quotation mark is used ( "..." ).
 * @param string String to quote
 * @param prefix Prefix
 * @param suffix Suffix
 * @return new string
 */
template <typename T,
          typename U = typename std::decay_t<T>::value_type,
          typename W = U>
inline auto
quoted(T&& string, U&& prefix = '"', W&& suffix = '"')
{
    return std::move(prefix) + std::forward<T>(string) + std::move(suffix);
}

/**
 * @brief Puts a string type in single quotation marks ( '...' ).
 * @param string String to quote
 * @return new string
 */
template <typename T, typename U = typename std::decay_t<T>::value_type>
inline auto
squoted(T&& string)
{
    return gt::quoted(std::forward<T>(string), U{'\''}, U{'\''});
}

/**
 * @brief Puts a string type in brackets ( (...) ).
 * @param string String to put in brackets
 * @return new string
 */
template <typename T, typename U = typename std::decay_t<T>::value_type>
inline auto
brackets(T&& string)
{
    return gt::quoted(std::forward<T>(string), U{'('}, U{')'});
}

/**
 * @brief Helper method for returning a value and setting the 'ok' param in one
 * call
 * @param t Type to return
 * @param success Value to set 'ok' to
 * @param ok Ok ptr
 * @return t
 */
template<typename T, typename TSuccess>
decltype(auto) valueSetSuccess(T&& t, TSuccess success, TSuccess* ok)
{
    if (ok)
    {
        *ok = std::move(success);
    }
    return std::forward<T>(t);
}

/**
 * @brief Helper method for returning a value and setting the 'ok' param to
 * true if its not null.
 * @param t Type to return
 * @param ok Ok ptr to set to true
 * @return t
 */
template<typename T>
decltype(auto) valueSuccess(T&& t, bool* ok)
{
    return valueSetSuccess(std::forward<T>(t), true, ok);
}

/**
 * @brief Helper method for returning a value and setting the 'ok' param to
 * false if its not null.
 * @param t Type to return
 * @param ok Ok ptr to set to false
 * @return t
 */
template<typename T>
decltype(auto) valueError(T&& t, bool* ok)
{
    return valueSetSuccess(std::forward<T>(t), false, ok);
}

/**
 * @brief Checks against T's numerical limits
 * @tparam T Target type
 * @param u Value to check
 * @return is u within t's umerical limits
 */
template <typename T, typename U>
inline bool
checkNumericalLimits(U u)
{
    return !(u < static_cast<U>(std::numeric_limits<T>::min()) ||
             u > static_cast<U>(std::numeric_limits<T>::max()));
}

/**
 * @brief clamp - ensure a value to be between to boarder values
 * @param value to clamp
 * @param low boarder value
 * @param high border value
 * @return value if it is between low and high, if less than low return low,
 * if higher than high return high
 *
 * C++17 will introduce a std::clamp() function.
 */
template<typename T>
T const& clamp(T const& value, T const& low, T const& high)
{
    return std::max(low, std::min(high, value));
}

} // namespace gt

#endif // GT_UTILITIES_H
