/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLISTVIEW_H
#define GTLISTVIEW_H

#include "gt_mdi_exports.h"

#include <QListView>

/**
 * @brief The GtListView class - inhereted class of QListView
 * modified version to overwrite protected Events
 */
class GT_MDI_EXPORT GtListView : public QListView
{
    Q_OBJECT

public:
    /**
     * @brief GtListView - Constructor identical to QListView
     * @param parent - Widget as parent of view
     */
    GtListView(QWidget* parent = Q_NULLPTR);

protected:
    virtual void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

signals:
    /**
     * @brief searchRequest - signal as request to search in listview
     *  signal is sent when pressing the Key "Control + F"
     */
    void searchRequest();

    /**
     * @brief copyRequest - signal as request to copy lines in listview
     *  signal is sent when pressing the Key "Control + C"
     */
    void copyRequest();

    /**
     * @brief deleteRequest - signal as request to remove lines in listview
     *  signal is sent when pressing the Key "Delete"
     */
    void deleteRequest();

};

#endif // GTLISTVIEW_H