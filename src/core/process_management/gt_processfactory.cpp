/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
    if (retval == nullptr)
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
