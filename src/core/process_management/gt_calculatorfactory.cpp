/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>

#include "gt_calculatorfactory.h"

#include "gt_tasklink.h"
#include "gt_calculatordata.h"

GtCalculatorFactory::GtCalculatorFactory(QObject* parent) : QObject(parent)
{
    registerClass(GT_METADATA(GtTaskLink));
}

GtCalculatorFactory*
GtCalculatorFactory::instance()
{
    static GtCalculatorFactory* retval = nullptr;
    if (!retval)
    {
        retval = new GtCalculatorFactory(qApp);
    }
    return retval;
}

bool
GtCalculatorFactory::registerCalculatorData(GtCalculatorData calcData)
{
    if (!registerClass(calcData->metaData()))
    {
        return false;
    }

    m_calcData.insert(calcData->metaData().className(), calcData);

    return true;
}

QList<GtCalculatorData>
GtCalculatorFactory::calculatorDataList() const
{
    return m_calcData.values();
}

GtCalculatorData
GtCalculatorFactory::calculatorData(const QString& className)
{
    if (m_calcData.contains(className))
    {
        return m_calcData.value(className);
    }

    return GtCalculatorData();
}

bool
GtCalculatorFactory::calculatorDataExists(const QString& className)
{
    return m_calcData.contains(className);
}
