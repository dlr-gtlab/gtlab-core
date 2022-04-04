/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 15.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_packageui.h"

#include "gt_icons.h"

GtPackageUI::GtPackageUI()
{

}

QIcon
GtPackageUI::icon(GtObject* /*obj*/) const
{
    return GtGUI::icon("pluginIcon_16.png");
}

