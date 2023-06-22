/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractprocessdata.cpp
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

GtAbstractProcessData::GtAbstractProcessData() :
    status(PROTOTYPE)
{
    // nothing to do here
}
