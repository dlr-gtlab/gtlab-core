/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractbrowserwidget.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 16.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_ABSTRACTBROWSERWIDGET_H
#define GT_ABSTRACTBROWSERWIDGET_H

#include "gt_gui_exports.h"

#include <QWidget>

class GtCollectionNetworkItem;
/**
 * @brief The GtAbstractBrowserWidget class
 */
class GT_GUI_EXPORT GtAbstractBrowserWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtAbstractBrowserWidget
     * @param parent
     */
    explicit GtAbstractBrowserWidget(QWidget* parent = nullptr);

    /**
     * @brief itemsToDownload
     * @return
     */
    virtual QList<GtCollectionNetworkItem> itemsToDownload() = 0;

    /**
     * @brief itemsToUpdate
     * @return
     */
    virtual QList<GtCollectionNetworkItem> itemsToUpdate() = 0;

    /**
     * @brief selectAllItems
     */
    virtual void selectAllItems() = 0;

    /**
     * @brief unselectAllItems
     */
    virtual void unselectAllItems() = 0;

signals:
    /**
     * @brief selectionChanged
     */
    void selectionChanged();

};

#endif // GT_ABSTRACTBROWSERWIDGET_H
