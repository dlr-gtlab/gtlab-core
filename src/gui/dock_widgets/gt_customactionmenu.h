/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 24.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCUSTOMACTIONMENU_H
#define GTCUSTOMACTIONMENU_H

#include <QObject>
#include <QHash>
#include "gt_gui_exports.h"
#include "gt_objectuiaction.h"

class QSignalMapper;
class QAction;
class QMenu;
class GtObject;
class GtObjectUI;

/**
 * @brief The GtCustomActionMenu class
 */
class GT_GUI_EXPORT GtCustomActionMenu : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Appends the actions in list to the menu. Actions will be invoked
     * automatically once triggered
     * @param list List of actions to append
     * @param targetObject Target object
     * @param parentObject Parent object
     * @param menu Menu
     */
    GtCustomActionMenu(const QList<GtObjectUIAction>& list,
                       GtObject* targetObject,
                       QObject* parentObject,
                       QMenu* menu);

private:
    /// mapper for action signals
    QSignalMapper* m_signalMapper;

    /// List of action associated with mdi items
    QHash<QAction*, GtObjectUIAction> m_actions;

    /// Object data
    GtObject* m_targetObj;

    /// Parent object
    QObject* m_parentObj;

private slots:
    /**
     * @brief Called after trigger of an action
     * @param returned Action
     */
    void onActionTrigger(QObject* obj);

};

#endif // GTCUSTOMACTIONMENU_H
