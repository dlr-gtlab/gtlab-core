/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractcollectionsettings.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 07.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_ABSTRACTCOLLECTIONSETTINGS_H
#define GT_ABSTRACTCOLLECTIONSETTINGS_H

#include "gt_mdi_exports.h"

#include <QList>
#include <QObject>

class QWidget;
class GtCollectionItem;
class GtCollectionNetworkItem;
class GtAbstractBrowserWidget;

/**
 * @brief The GtAbstractCollectionSettings class
 */
class GT_MDI_EXPORT GtAbstractCollectionSettings : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief GtAbstractCollectionSettings
     */
    GtAbstractCollectionSettings();

    /**
     * @brief localCollectionWidget
     * @param parent
     */
    virtual QWidget* localCollectionWidget(QWidget* parent) = 0;

    /**
     * @brief setLocalCollectionData
     * @param editor
     * @param items
     */
    virtual void setLocalCollectionData(QWidget* widget,
                                   QList<GtCollectionItem> items) = 0;

    /**
     * @brief browseCollectionWidget
     * @param parent
     */
    virtual GtAbstractBrowserWidget* browserWidget(QWidget* parent) = 0;

    /**
     * @brief setBrowserWidgetData
     * @param editor
     * @param items
     */
    virtual void setBrowserWidgetData(GtAbstractBrowserWidget* widget,
                                      QList<GtCollectionNetworkItem> installedItems,
                                      QList<GtCollectionNetworkItem> availableItems,
                                      QList<GtCollectionNetworkItem> updataAvailableItems) = 0;

};

#endif // GT_ABSTRACTCOLLECTIONSETTINGS_H
