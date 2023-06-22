/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLISTVIEW_H
#define GTLISTVIEW_H

#include "gt_gui_exports.h"

#include <QListView>

/**
 * @brief The GtListView class - inhereted class of QListView
 * modified version to overwrite protected Events
 */
class GT_GUI_EXPORT GtListView : public QListView
{
    Q_OBJECT

public:
    /**
     * @brief GtListView - Constructor identical to QListView
     * @param parent - Widget as parent of view
     */
    explicit GtListView(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;

signals:
    /**
     * @brief searchRequest - signal as request to search in the view
     *  signal is sent when pressing the search shortcut
     */
    void searchRequest();

    /**
     * @brief copyRequest - signal as request to copy lines in the view
     *  signal is sent when pressing the copy shortcut
     */
    void copyRequest();

    /**
     * @brief deleteRequest - signal as request to remove lines in the view
     *  signal is sent when pressing the delete shortcut
     */
    void deleteRequest();

};

#endif // GTLISTVIEW_H
