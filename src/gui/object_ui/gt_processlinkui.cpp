/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
    return gt::gui::icon::process();
}

