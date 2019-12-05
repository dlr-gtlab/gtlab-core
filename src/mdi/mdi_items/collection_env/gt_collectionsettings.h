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

#include "gt_mdi_exports.h"

#include "gt_abstractcollectionsettings.h"

/**
 * @brief The GtCollectionSettings class
 */
class GT_MDI_EXPORT GtCollectionSettings : public GtAbstractCollectionSettings
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
    virtual QWidget* localCollectionWidget(QWidget* parent) Q_DECL_OVERRIDE;

    /**
     * @brief setLocalCollectionData
     * @param editor
     * @param items
     */
    virtual void setLocalCollectionData(QWidget* widget,
                                        QList<GtCollectionItem> items) Q_DECL_OVERRIDE;

    /**
     * @brief browseCollectionWidget
     * @param parent
     */
    virtual GtAbstractBrowserWidget* browserWidget(QWidget* parent) Q_DECL_OVERRIDE;

    /**
     * @brief setBrowserWidgetData
     * @param widget
     * @param installedItems
     * @param availableItems
     * @param updataAvailableItems
     */
    virtual void setBrowserWidgetData(GtAbstractBrowserWidget* widget,
                                      QList<GtCollectionNetworkItem> installedItems,
                                      QList<GtCollectionNetworkItem> availableItems,
                                      QList<GtCollectionNetworkItem> updataAvailableItems) Q_DECL_OVERRIDE;

};

#endif // GT_COLLECTIONSETTINGS_H
