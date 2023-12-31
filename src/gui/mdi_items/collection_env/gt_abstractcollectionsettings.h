/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractcollectionsettings.h
 *
 *  Created on: 07.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_ABSTRACTCOLLECTIONSETTINGS_H
#define GT_ABSTRACTCOLLECTIONSETTINGS_H

#include "gt_gui_exports.h"

#include <QList>
#include <QObject>

class QWidget;
class GtCollectionItem;
class GtCollectionNetworkItem;
class GtAbstractBrowserWidget;

/**
 * @brief The GtAbstractCollectionSettings class
 */
class GT_GUI_EXPORT GtAbstractCollectionSettings : public QObject
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
        const QList<GtCollectionNetworkItem>& installedItems,
        const QList<GtCollectionNetworkItem>& availableItems,
        const QList<GtCollectionNetworkItem>& updataAvailableItems) = 0;

signals:
    /**
     * @brief themeChanged - emit signal for changed theme
     */
    void themeChanged();
};

#endif // GT_ABSTRACTCOLLECTIONSETTINGS_H
