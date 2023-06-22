/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTABLEVIEW_H
#define GTTABLEVIEW_H

#include "gt_gui_exports.h"

#include <QTableView>

/**
 * @brief The GtTableView class
 */
class GT_GUI_EXPORT GtTableView : public QTableView
{
    Q_OBJECT

public:
    explicit GtTableView(QWidget* parent = nullptr);

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

#endif // GTTABLEVIEW_H
