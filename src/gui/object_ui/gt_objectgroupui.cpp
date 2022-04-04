/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 15.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_objectgroupui.h"
#include "gt_icons.h"

GtObjectGroupUI::GtObjectGroupUI()
{

}

QIcon
GtObjectGroupUI::icon(GtObject* /*obj*/) const
{
    return GtGUI::icon("folder_16.png");
}

