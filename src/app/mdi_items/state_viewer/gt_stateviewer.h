/* GTlab - Gas Turbine laboratory
 * Source File: gt_stateviewer.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSTATEVIEWER_H
#define GTSTATEVIEWER_H

#include "gt_mdiitem.h"

class QTreeView;
class GtStateModel;

/**
 * @brief The GtStateViewer class
 */
class GtStateViewer : public GtMdiItem
{
    Q_OBJECT

public:
    /**
     * @brief GtStateViewer
     */
    Q_INVOKABLE GtStateViewer();

    /**
     * @brief Virtual function to specify item specific icon.
     * @return Object specific icon.
     */
    virtual QIcon icon() const override;

public slots:
    /**
     * @brief onThemeChanged - how to handle theme change
     */
    void onThemeChanged() override;
private:
    /// Model
    GtStateModel* m_model;

    /// Tree view
    QTreeView* m_tree;

private slots:
    /**
     * @brief Reloads model and expands tree.
     */
    void reloadModel();

};

#endif // GTSTATEVIEWER_H
