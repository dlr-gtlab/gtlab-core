/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_variantconvert.h
 *
 *  Created on: 01.04.2022
 *  Author: Martin Siggel (AT-TWK)
 *  Tel.: +49 2203 601 2264
 */

#ifndef VARIANT_CONVERT_H
#define VARIANT_CONVERT_H

#include <QVariant>
#include <string>

#include "gt_mpl.h"
#include "gt_typetraits.h"

#include <stdexcept>

namespace gt
{

/// can convert
template <typename TargetType>
inline bool canConvert(const QVariant& v)
{
    static_assert (!trait::is_generic_pointer<TargetType>::value,
                   "Non QObject-Pointers cannot be converted to QVariant!");
    return v.canConvert<TargetType>();
}

/**
 * restrict automatic conversions
 */
template <>
inline bool canConvert<QByteArray>(const QVariant& v)
{
    return (v.type() == QVariant::String || v.type() == QVariant::ByteArray) && v.canConvert<QByteArray>();
}

template <>
inline bool canConvert<QString>(const QVariant& v)
{
    return v.type() == QVariant::String && v.canConvert<QString>();
}

template <>
inline bool canConvert<bool>(const QVariant& v)
{
    return v.type() == QVariant::Bool;
}

template <>
inline bool canConvert<double>(const QVariant& v)
{
    bool ok = true;
    v.toDouble(&ok);
    return ok;
}

template <>
inline bool canConvert<float>(const QVariant& v)
{
    bool ok = true;
    v.toFloat(&ok);
    return ok;
}

template <>
inline bool canConvert<int>(const QVariant& v)
{
    bool ok = true;
    v.toInt(&ok);
    return ok;
}

template <>
inline bool canConvert<unsigned int>(const QVariant& v)
{
    bool ok = true;
    v.toUInt(&ok);
    return ok;
}

template <>
inline bool canConvert<long long>(const QVariant&  v)
{
    bool ok = true;
    v.toLongLong(&ok);
    return ok;
}

template <>
inline bool canConvert<unsigned long long>(const QVariant&  v)
{
    bool ok = true;
    v.toULongLong(&ok);
    return ok;
}

/// to variant
template <typename T>
inline QVariant toVariant(const T& t)
{
    static_assert (!trait::is_generic_pointer<T>::value,
                   "Non QObject-Pointers cannot be converted to QVariant!");
    return QVariant::fromValue(t);
}

/**
 * custom conversions
 */
template <>
inline bool canConvert<std::string>(const QVariant& v)
{
    return canConvert<QString>(v);
}

template<>
inline QVariant toVariant(const std::string& s)
{
    return toVariant(QString::fromStdString(s));
}

/// to variant list
template <typename T>
inline QVariantList toVariantList(const T& t)
{
    return {toVariant(t)};
}

template <typename ...Args>
inline QVariantList toVariantList(const std::tuple<Args...>& t)
{
    QVariantList list;
    mpl::static_foreach(t, [&list](const auto & v) {
        list.append(toVariant(v));
    });
    return list;
}

inline QVariantList toVariantList(mpl::detail::void_type)
{
    return {};
}

/// from variant
template <typename T>
inline T fromVariant(const QVariant& v)
{
    static_assert (!trait::is_generic_pointer<T>::value,
                   "Non QObject-Pointers cannot be converted to QVariant!");
    return v.value<T>();
}

template <>
inline std::string fromVariant<std::string>(const QVariant& v)
{
    return v.toString().toStdString();
}

namespace detail
{

template <class T>
struct from_variant_impl_{};

template <typename ... Tuple>
struct from_variant_impl_<std::tuple<Tuple...>>
{
    // remove const and references from all arguments
    typedef std::tuple<std::decay_t<Tuple>...> type;

    static type convert(const QVariantList& variant_list)
    {
        type tuple;

        constexpr size_t tuple_size = std::tuple_size<type>::value;
        assert(tuple_size == variant_list.size());

        mpl::static_for<tuple_size>(
            [&tuple, &variant_list](const auto j)
            {
                using ArgType = std::tuple_element_t<j, type>;

                const auto& variant_arg = variant_list.at(j);
                if (!canConvert<ArgType>(variant_arg)) {
                    throw std::runtime_error(
                        "Cannot convert function argument " +
                        std::to_string(j) + ". Expecting type '" +
                        typeid(ArgType).name() + "' got value: " +
                        variant_arg.toString().toStdString()
                        );
                }

                std::get<j>(tuple) = fromVariant<ArgType>(variant_arg);
            });

        return tuple;
    }
};

} // namespace detail

/**
 * @brief Converts a QVariantList to a tuple
 */
template <typename ReturnTupleType>
inline typename detail::from_variant_impl_<ReturnTupleType>::type fromVariant(const QVariantList& l)
{
    return detail::from_variant_impl_<ReturnTupleType>::convert(l);
}

} // namespace gt

#endif // VARIANT_CONVERT_H
