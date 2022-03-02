/* GTlab - Gas Turbine laboratory
 * Source File: gt_calculatordata.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 11.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QObject>

#include "gt_calculatordata.h"

GtCalculatorDataImpl::GtCalculatorDataImpl(const QMetaObject& metaData) :
    m_metaData(metaData)
{
    description.clear();
    status = PROTOTYPE;
}

const QMetaObject&
GtCalculatorDataImpl::metaData() const
{
    return m_metaData;
}

bool
GtCalculatorDataImpl::isValid() const
{
    if (version.isNull())
    {
        return false;
    }

    if (id.isEmpty())
    {
        return false;
    }

    if (author.isEmpty())
    {
        return false;
    }

    QObject* obj = m_metaData.newInstance();

    if (!obj)
    {
        return false;
    }

    delete obj;

    return true;
}

GtCalculatorData
GtCalculatorDataImpl::newCalcData(const QMetaObject& metaData)
{
    return GtCalculatorData(new GtCalculatorDataImpl(metaData));
}

void
GtCalculatorDataImpl::addEnvironmentVariable(const QString& var)
{
    if (m_envVars.contains(var))
    {
        return;
    }

    m_envVars << var;
}

const QStringList&
GtCalculatorDataImpl::environmentVariables()
{
    return m_envVars;
}
