/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_doublelistproperty.cpp
 *
 *  Created on: 23.01.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_doublelistproperty.h"

#include <QSequentialIterable>

GtDoubleListProperty::GtDoubleListProperty(const QString& ident,
        const QString& name)
{
    setObjectName(name);

    m_id = ident;
    m_brief = QString();
    m_unitCategory = GtUnit::Category::None;
    m_value = QVector<double>();
    m_initValue = m_value;
}

QVariant
GtDoubleListProperty::valueToVariant(const QString& /*unit*/,
                                     bool* success) const
{
    QVariant variant = QVariant::fromValue(m_value);

    return gt::valueSuccess(variant, success);
}

bool
GtDoubleListProperty::setValueFromVariant(const QVariant& val,
                                          const QString& /*unit*/)
{
    if (!val.canConvert<QVariantList>()) return false;

    QSequentialIterable const iterable = val.value<QSequentialIterable>();
    QVector<double> vec;

    for (const QVariant& v : iterable)
    {
        bool ok = false;
        vec << v.toDouble(&ok);

        if (!ok)
        {
            gtError() << tr("Could not convert double list property!");
            return false;
        }
    }

    bool ok = false;;
    setVal(vec, &ok);
    return ok;
}

QString
GtDoubleListProperty::valuesToString() const
{
    QString retval;

    for (int i = 0; i < m_value.size(); ++i)
    {
        retval = retval + QString::number(m_value[i]);

        if (i < m_value.size() - 1)
        {
            retval = retval + QStringLiteral(";");
        }
    }

    return retval;
}

bool
GtDoubleListProperty::setValueFromString(const QString& val)
{
    QStringList const strList = val.split(QStringLiteral(";"));

    QVector<double> tmp;
    bool success = false;

    for (const QString& str : strList)
    {
        double tmpDouble = str.toDouble(&success);

        if (!success)
        {
            return false;
        }

        tmp << tmpDouble;
    }

    setVal(tmp, &success);

    return success;
}

gt::PropertyFactoryFunction
gt::makeDoubleListProperty()
{
    return [](const QString& id)
    {
        return new GtDoubleListProperty(id, id);
    };
}
