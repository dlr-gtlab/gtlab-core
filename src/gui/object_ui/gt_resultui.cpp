/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_resultui.h"
#include "gt_icons.h"

GtResultUI::GtResultUI()
{

}

QIcon
GtResultUI::icon(GtObject* /*obj*/) const
{
    return GtGUI::Icon::data16();
}

