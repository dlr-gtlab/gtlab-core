/* GTlab - Gas Turbine laboratory
 * Source File: gt_exportmenu.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTEXPORTMENU_H
#define GTEXPORTMENU_H

#include <QMenu>

class QSignalMapper;
class GtObject;

/**
 * @brief The GtExportMenu class
 */
class GtExportMenu : public QMenu
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Object for exporter menu.
     * @param Parent widget.
     */
    GtExportMenu(GtObject* obj,
                 QWidget* parent = Q_NULLPTR);

private:
    /// mapper for action signals
    QSignalMapper* m_signalMapper;

    /// List of action associated with exporter
    QHash<QAction*, QString> m_actions;

    /// Object data
    GtObject* m_obj;

private slots:
    /**
     * @brief Called after trigger of an action
     * @param returned Action
     */
    void onActionTrigger(QObject* obj);

};

#endif // GTEXPORTMENU_H
