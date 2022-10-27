/* GTlab - Gas Turbine laboratory
 * Source File: gt_labelui.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 20.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_icons.h"

#include "gt_labelui.h"

GtLabelUI::GtLabelUI()
{

}

QIcon
GtLabelUI::icon(GtObject* /*obj*/) const
{
    return gt::gui::icon::labels();
}
