/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 07.01.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_processlinkui.h"
#include "gt_icons.h"

GtProcessLinkUI::GtProcessLinkUI()
{

}

QIcon
GtProcessLinkUI::icon(GtObject* /*obj*/) const
{
    return GtGUI::icon(QStringLiteral("processIcon_16.png"));
}

