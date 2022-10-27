/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 23.9.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GT_UTILITIES_H
#define GT_UTILITIES_H

#include <utility>

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

} // namespace gt

#endif // GT_UTILITIES_H
