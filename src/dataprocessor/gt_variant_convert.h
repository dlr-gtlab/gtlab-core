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
bool can_convert(const QVariant&  v)
{
    return false;
}

template <>
bool can_convert<double>(const QVariant&  v)
{
    bool ok = true;
    v.toDouble(&ok);

    return ok;
}

template <>
bool can_convert<float>(const QVariant&  v)
{
    bool ok = true;
    v.toFloat(&ok);

    return ok;
}

template <>
bool can_convert<int>(const QVariant&  v)
{
    bool ok = true;
    v.toInt(&ok);

    return ok;
}

template <typename T>
QVariant to_variant(const T& t)
{
    return QVariant(t);
}

template<>
QVariant to_variant(const std::string& s)
{
    return QVariant(s.c_str());
}


template <typename T>
QVariantList to_variant_list(const T& t)
{
    return {to_variant(t)};
}

template <typename ...Args>
QVariantList to_variant_list(const std::tuple<Args...>& t)
{
    QVariantList list;
    mpl::static_foreach(t, [&list](const auto & v) {
        list.append(to_variant(v));
    });
    return list;
}


QVariantList to_variant_list(mpl::detail::void_type)
{
    return {};
}

#endif // VARIANT_CONVERT_H
