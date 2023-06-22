/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 17.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOPENWITHMENU_H
#define GTOPENWITHMENU_H

#include <QMenu>

class QSignalMapper;
class GtObject;

/**
 * @brief The GtOpenWithMenu class
 */
class GtOpenWithMenu : public QMenu
{
    Q_OBJECT

public:
    /**
     * @brief GtOpenWithMenu
     * @param list
     * @param parent
     */
    GtOpenWithMenu(const QStringList& list, GtObject* obj,
                   QWidget* parent = nullptr);

private:
    /// mapper for action signals
    QSignalMapper* m_signalMapper;

    /// List of action associated with mdi items
    QHash<QAction*, QString> m_actions;

    /// Object data
    GtObject* m_obj;

    /**
     * @brief cleanMenuName
     * The name of the action entry in the context menu can be changed with this
     * function.
     * A list of names is handled in this function
     * @param className: the original name
     * @return a string defined as a substitute for the original class name
     */
    QString cleanMenuName(const QString& className);
private slots:
    /**
     * @brief Called after trigger of an action
     * @param returned Action
     */
    void onActionTrigger(QObject* obj);

};

#endif // GTOPENWITHMENU_H
