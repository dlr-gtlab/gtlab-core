/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractprocessdata.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 11.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QString>

#include "gt_abstractprocessdata.h"

GtAbstractProcessData::~GtAbstractProcessData()
{
    // nothing to do here
}

QString
GtAbstractProcessData::statusToString(GtAbstractProcessData::DEV_STATUS s)
{
    switch (s)
    {
        case RELEASE:
            return QStringLiteral("RELEASE");
        case DEPRECATED:
            return QStringLiteral("DEPRECATED");
        case PROTOTYPE:
            return QStringLiteral("PROTOTYPE");
    }

    return QString();
}

GtAbstractProcessData::GtAbstractProcessData()
{
    // nothing to do here
}
