/* GTlab - Gas Turbine laboratory
 * Source File: gt_importmenu.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 08.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTIMPORTMENU_H
#define GTIMPORTMENU_H

#include <QMenu>

#include "gt_gui_exports.h"

class QSignalMapper;
class GtObject;

/**
 * @brief The GtImportMenu class
 */
class GT_GUI_EXPORT GtImportMenu : public QMenu
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Object for importer menu.
     * @param Parent widget.
     */
    explicit GtImportMenu(GtObject *obj, QWidget *parent = nullptr);

private:
    /// mapper for action signals
    QSignalMapper* m_signalMapper;

    /// List of action associated with importer
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

#endif // GTIMPORTMENU_H
