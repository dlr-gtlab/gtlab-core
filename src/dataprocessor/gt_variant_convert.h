/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_dynamicinterface.cpp
 * copyright 2022 by DLR
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

template <typename TargetType>
inline bool can_convert(const QVariant&  /*v*/)
{
    return false;
}

template <>
inline bool can_convert<QVariant>(const QVariant&  /*v*/)
{
    return true;
}

template <>
inline bool can_convert<double>(const QVariant&  v)
{
    bool ok = true;
    v.toDouble(&ok);

    return ok;
}

template <>
inline bool can_convert<float>(const QVariant&  v)
{
    bool ok = true;
    v.toFloat(&ok);

    return ok;
}

template <>
inline bool can_convert<int>(const QVariant&  v)
{
    bool ok = true;
    v.toInt(&ok);

    return ok;
}


template <>
inline constexpr bool can_convert<QString>(const QVariant& )
{
    return true;
}

template <>
inline constexpr bool can_convert<std::string>(const QVariant& )
{
    return true;
}


template <typename T>
inline QVariant to_variant(const T& t)
{
    return QVariant(t);
}

template<>
inline QVariant to_variant(const std::string& s)
{
    return QVariant(s.c_str());
}


template <typename T>
inline QVariantList to_variant_list(const T& t)
{
    return {to_variant(t)};
}

template <typename ...Args>
inline QVariantList to_variant_list(const std::tuple<Args...>& t)
{
    QVariantList list;
    mpl::static_foreach(t, [&list](const auto & v) {
        list.append(to_variant(v));
    });
    return list;
}


inline QVariantList to_variant_list(mpl::detail::void_type)
{
    return {};
}

template <typename T>
inline T from_variant(const QVariant& v)
{
    return v.value<T>();
}

template <>
inline std::string from_variant<std::string>(const QVariant& v)
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
                if (!can_convert<ArgType>(variant_arg)) {
                    throw std::runtime_error(
                        "Cannot convert function argument " +
                        std::to_string(j) + ". Expecting type '" +
                        typeid(ArgType).name() + "' got value: " +
                        variant_arg.toString().toStdString()
                        );
                }

                std::get<j>(tuple) = from_variant<ArgType>(variant_arg);
            });

        return tuple;
    }
};

} // namespace detail

/**
 * @brief Converts a QVariantList to a tuple
 */
template <typename ReturnTupleType>
inline typename detail::from_variant_impl_<ReturnTupleType>::type from_variant(const QVariantList& l)
{
    return detail::from_variant_impl_<ReturnTupleType>::convert(l);
}


#endif // VARIANT_CONVERT_H
