/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processoverviewmodel.h
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_PROCESSOVERVIEWMODEL_H
#define GT_PROCESSOVERVIEWMODEL_H

#include "gt_versionnumber.h"
#include <QAbstractItemModel>

class GtProcessCategoryItem;
class GtAbstractProcessItem;

namespace gt {

struct ProcessInformation
{
    /// Calculator identification string.
    QString id;

    /// Calculator version.
    GtVersionNumber version;

    /// Calculator description.
    QString description{"-"};

    /// Calculator author.
    QString author{"-"};

    /// Calculator author contact.
    QString contact{"-"};

    /// Calculator author company.
    QString company{"-"};

    /// Calculator category.
    QString category{"-"};
};

} // namespace gt

/**
 * @brief The GtProcessOverviewModel class
 */
class GtProcessOverviewModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Roles
    {
        CategoryRole = Qt::UserRole + 1,
        CollapseRole,
        ButtonRole
    };

    /**
     * @brief Constructor.
     * @param parent Parent object.
     */
    explicit GtProcessOverviewModel(QObject* parent = nullptr);

    /**
     *  Destructor
     */
    ~GtProcessOverviewModel() override;

    /**
     * @brief Virtual function to implement specific model setup.
     */
    virtual void setUpModel() = 0;

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(
            const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    int columnCount(
            const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex &index, int role) const override;

    /**
     * @brief setData
     * @param index
     * @param value
     * @param role
     * @return
     */
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role) override;

    /**
     * @brief index
     * @param row
     * @param col
     * @param parent
     * @return
     */
    QModelIndex index(int row,
                      int col,
                      const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief parent
     * @param index
     * @return
     */
    QModelIndex parent(const QModelIndex& index) const override;

    /**
     * @brief itemFromIndex
     * @param index
     * @return
     */
    GtAbstractProcessItem* itemFromIndex(const QModelIndex& index) const;

protected:
    /**
     * @brief Returns identification string of given process item.
     * @param item Process item.
     * @return Identification string of process item.
     */
    virtual QString id(GtAbstractProcessItem* item) const = 0;

    /**
     * @brief Returns version string of given process item.
     * @param item Process item.
     * @return Version string of process item.
     */
    virtual QString version(GtAbstractProcessItem* item) const = 0;

    /**
     * @brief Returns icon of given process item.
     * @param item Process item.
     * @return Icon of process item.
     */
    virtual QIcon icon(GtAbstractProcessItem* item) const = 0;

    /**
     * @brief Returns description of given process item.
     * @param item Process item.
     * @return description of process item.
     */
    virtual QString description(GtAbstractProcessItem* item) const = 0;

    /**
     * @brief Returns a process information struct containing all relevant data
     * @param item
     * @return
     */
    virtual gt::ProcessInformation processElementInformation(
        GtAbstractProcessItem* item) const = 0;

    /**
     * @brief categoryItem
     * @param id
     * @return
     */
    GtProcessCategoryItem* categoryItem(const QString& id);

public slots:
    /**
     * @brief onButtonClicked
     * @param item
     */
    void onButtonClicked(GtAbstractProcessItem* item,
                         const QPoint& globalPos);

private:
    /// Categories and calculator data
    QList<GtProcessCategoryItem*> m_categories;

    /**
     * @brief indexFromItem
     * @param item
     * @return
     */
    QModelIndex indexFromItem(GtAbstractProcessItem* item) const;
};

#endif // GT_PROCESSOVERVIEWMODEL_H
