/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
    return gt::gui::icon::folder();
}

