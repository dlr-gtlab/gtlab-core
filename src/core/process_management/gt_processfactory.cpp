/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>

#include "gt_processfactory.h"
#include "gt_taskfactory.h"
#include "gt_calculatorfactory.h"
#include "gt_objectfactory.h"

GtProcessFactory::GtProcessFactory(QObject* parent) : QObject(parent)
{
    addFactory(gtTaskFactory);
    addFactory(gtCalculatorFactory);
    addFactory(gtObjectFactory);
}

GtProcessFactory*
GtProcessFactory::instance()
{
    static GtProcessFactory* retval = nullptr;
    if (!retval)
    {
        retval = new GtProcessFactory(qApp);
    }
    return retval;
}

GtAbstractObjectFactory*
GtProcessFactory::taskFactory()
{
    return gtTaskFactory;
}

GtAbstractObjectFactory*
GtProcessFactory::calculatorFactory()
{
    return gtCalculatorFactory;
}
