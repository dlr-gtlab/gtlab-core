/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 21.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTREEVIEW_H
#define GTTREEVIEW_H

#include "gt_gui_exports.h"

#include <QTreeView>

/**
 * @brief The GtTreeView class
 */
class GT_GUI_EXPORT GtTreeView : public QTreeView
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Parent widget.
     */
    explicit GtTreeView(QWidget* parent = nullptr);

protected:
    /**
     * @brief keyPressEvent
     * @param event
     */
    void keyPressEvent(QKeyEvent* event) override;

signals:
    /**
     * @brief searchRequest
     */
    void searchRequest();

    /**
     * @brief copyRequest
     */
    void copyRequest();

};

#endif // GTTREEVIEW_H
