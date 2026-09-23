/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 24.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QMenu>

#include "gt_customactionmenu.h"
#include "gt_object.h"
#include "gt_guiutilities.h"

GtCustomActionMenu::GtCustomActionMenu(const QList<GtObjectUIAction>& list,
                                       GtObject* targetObject,
                                       QObject* parentObject,
                                       QMenu* menu) :
    QObject(menu),
    m_targetObj(targetObject),
    m_parentObj(parentObject)
{
    if (menu) gt::gui::addToMenu(list, *menu, targetObject, parentObject);
}
