/* GTlab - Gas Turbine laboratory
 * Source File: gt_doublelistproperty.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 23.01.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_doublelistproperty.h"

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
GtDoubleListProperty::valueToVariant(
        const QString& /*unit*/, bool* /*success*/) const
{
    QVariant variant = QVariant::fromValue(m_value);

    return variant;
}

bool
GtDoubleListProperty::setValueFromVariant(const QVariant& val,
        const QString& /*unit*/,
        bool* success)
{
    bool retval = false;

    if (val.canConvert<QVariantList>())
    {
        QSequentialIterable iterable = val.value<QSequentialIterable>();
        QVector<double> vec;

        foreach (const QVariant& v, iterable)
        {
            vec << v.toDouble(&retval);

            if (!retval)
            {
                gtError() << tr("Could not convert double list property!");

                if (success)
                {
                    *success = retval;
                }

                return retval;
            }
        }

        setVal(vec, success);

        if (success)
        {
            retval = *success;
        }
    }

    return retval;
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
    QStringList strList = val.split(QStringLiteral(";"));

    QVector<double> tmp;
    bool success = false;

    foreach (const QString& str, strList)
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
