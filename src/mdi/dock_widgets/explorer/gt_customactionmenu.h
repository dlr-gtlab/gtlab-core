/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 24.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCUSTOMACTIONMENU_H
#define GTCUSTOMACTIONMENU_H

#include <QObject>
#include <QHash>

#include "gt_objectuiaction.h"

class QSignalMapper;
class QAction;
class QMenu;
class GtObject;
class GtObjectUI;

/**
 * @brief The GtCustomActionMenu class
 */
class GtCustomActionMenu : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief GtOpenWithMenu
     * @param list
     * @param parent
     */
    GtCustomActionMenu(const QList<GtObjectUIAction>& list, GtObject* targetObj,
                       QObject* parentObject, QMenu* menu);

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
