/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractloadinghelper.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 29.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QTimer>

#include "gt_logging.h"

#include "gt_abstractloadinghelper.h"

GtAbstractLoadingHelper::GtAbstractLoadingHelper()
{

}

GtAbstractLoadingHelper::~GtAbstractLoadingHelper()
{
}

void GtAbstractLoadingHelper::exec()
{
    run();
    emit finished();
}
