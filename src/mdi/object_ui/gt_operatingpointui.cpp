/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_operatingpointui.h"

#include "gt_application.h"

GtOperatingPointUI::GtOperatingPointUI()
{

}

QIcon
GtOperatingPointUI::icon(GtObject* /*obj*/) const
{
    return gtApp->icon(QStringLiteral("globalsIcon.png"));
}

