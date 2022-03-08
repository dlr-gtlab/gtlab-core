/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionsettings.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 07.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_COLLECTIONSETTINGS_H
#define GT_COLLECTIONSETTINGS_H

#include "gt_gui_exports.h"

#include "gt_abstractcollectionsettings.h"

/**
 * @brief The GtCollectionSettings class
 */
class GT_GUI_EXPORT GtCollectionSettings : public GtAbstractCollectionSettings
{
    Q_OBJECT

public:
    /**
     * @brief GtCollectionSettings
     */
    Q_INVOKABLE GtCollectionSettings();

    /**
     * @brief localCollectionWidget
     * @param parent
     */
    QWidget* localCollectionWidget(QWidget* parent) override;

    /**
     * @brief setLocalCollectionData
     * @param editor
     * @param items
     */
    void setLocalCollectionData(QWidget* widget,
                                QList<GtCollectionItem> items) override;

    /**
     * @brief browseCollectionWidget
     * @param parent
     */
    GtAbstractBrowserWidget* browserWidget(QWidget* parent) override;

    /**
     * @brief setBrowserWidgetData
     * @param widget
     * @param installedItems
     * @param availableItems
     * @param updataAvailableItems
     */
    void setBrowserWidgetData(GtAbstractBrowserWidget* widget,
                              QList<GtCollectionNetworkItem> installedItems,
                              QList<GtCollectionNetworkItem> availableItems,
                              QList<GtCollectionNetworkItem> updataAvailableItems) override;

};

#endif // GT_COLLECTIONSETTINGS_H
