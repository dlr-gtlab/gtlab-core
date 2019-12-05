/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_result.h"
#include "gt_label.h"
#include <QStringList>
#include "gt_datazonetable.h"
#include "gt_abstractdatazone.h"
#include "gt_label.h"
#include "gt_logging.h"
#include <QDebug>

GtResult::GtResult()
{
    setObjectName(tr("Results"));
}

QList<GtLabel*>
GtResult::labels()
{
    return findDirectChildren<GtLabel*>();
}

QStringList
GtResult::labelNames()
{
    QStringList retval;

    foreach (GtLabel* label, labels())
    {
        retval.append(label->objectName());
    }

    return retval;
}
