/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYMODEL_H
#define GTPROPERTYMODEL_H

#include "gt_gui_exports.h"

#include <QAbstractItemModel>
#include <QPointer>

class GtObject;
class GtPropertyCategoryItem;
class GtAbstractPropertyItem;
class GtAbstractProperty;
class GtPropertyStructContainer;

/**
 * @brief The GtPropertyModel class
 */
class GT_GUI_EXPORT GtPropertyModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Roles
    {
        UnitCategoryRole = Qt::UserRole + 1,
        CategoryRole,
        ReadOnlyRole,
        OptionalRole,
        ActiveRole,
        MonitoringRole,
        ContainerRole,
        DefaultCollapseRole
    };

    /**
     * @brief Constructor.
     * @param parent Parent object.
     */
    explicit GtPropertyModel(GtObject* scope, QObject* parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~GtPropertyModel() override;

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
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    /**
     * @brief flags
     * @param index
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    /**
     * @brief setData
     * @param index
     * @param value
     * @param role
     * @return
     */
    bool setData(const QModelIndex& index,
                 const QVariant& value,
                 int role = Qt::EditRole) override;

    /**
     * @brief index
     * @param row
     * @param col
     * @param parent
     * @return
     */
    QModelIndex index(int row,
                      int col,
                      const QModelIndex& parent =
                          QModelIndex()) const override;

    /**
     * @brief parent
     * @param index
     * @return
     */
    QModelIndex parent(const QModelIndex& index) const override;

    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

    /**
     * @brief setObject
     * @param obj
     */
    void setObject(GtObject* obj);

    /**
     * @brief Sets new object to display the property structure for. Here only
     * for one property struct container.
     * @param obj object to which the container belongs
     * @param container
     */
    void setObject(GtObject* obj, GtPropertyStructContainer& container);

    /**
     * @brief Adds a new container entry based on given entry type.
     * @param container Target container
     * @param entryType Entry type identification string
     * @return Model index of new created entry
     */
    QModelIndex addNewStructContainerEntry(GtPropertyStructContainer& container,
                                    const QString& entryType);

    /**
     * @brief Removes container entry for given index.
     * @param index
     */
    void removeStructContainerEntry(const QModelIndex& index);

    /**
     * @brief object
     * @return
     */
    GtObject* object();

    /**
     * @brief propertyFromIndex
     * @param index
     * @return
     */
    GtAbstractPropertyItem* propertyFromIndex(const QModelIndex& index) const;

    /**
     * @brief removeItem
     * @param item
     * @return
     */
    bool removeItem(GtAbstractPropertyItem* item);

    /**
     * @brief updateModeItem
     * @param index
     */
    void updateModeItem(const QModelIndex& index);

    /**
     * @brief indexFromItem
     * @param obj
     * @return
     */
    QModelIndex indexFromProperty(GtAbstractPropertyItem* obj) const;

    /**
     * @brief updatePropertyData
     * @param obj
     */
    void updatePropertyData(GtAbstractPropertyItem* obj);

    /**
     * @brief setScope
     * @param obj
     */
    void setScope(GtObject* obj);

    /**
     * @brief setCategoryFilter
     * @param filter
     */
    void setCategoryFilter(const QStringList& filter);

    /**
     * @brief canDropMimeData
     * Overwritten function to check if the given mime data is valid
     * to be dropped
     * @param data
     * @param action
     * @param row
     * @param column
     * @param parent
     * @return true if the data is allowed to be used
     */
    bool canDropMimeData(const QMimeData* data, Qt::DropAction action,
                         int row, int column,
                         const QModelIndex& parent) const override;
    /**
     * @brief dropMimeData
     * Overwritten function to handle drop of mime data
     * @param data
     * @param action
     * @param row
     * @param column
     * @param parent
     * @return true if the data could be handled
     */
    bool dropMimeData(const QMimeData* data,
                      Qt::DropAction action,
                      int row, int column,
                      const QModelIndex& parent) override;

    /**
     * @brief mimeTypes
     * @return
     */
    QStringList mimeTypes() const override;

private:
    /// pointer to current object
    QPointer<GtObject> m_obj;

    /// property wrapper items
    QList<GtPropertyCategoryItem*> m_properties;

    /// Scope of data representation
    QPointer<GtObject> m_scope;

    /// Category filter
    QStringList m_catFilter;

    /// Container identification string. Only set if property strcut container
    /// is desplayed.
    QString m_containerId;

    /**
     * @brief addProperty
     * @param prop
     */
    void addProperty(GtAbstractProperty* prop);

    /**
     * @brief filteredCategories
     * @return
     */
    QList<GtPropertyCategoryItem*> filteredCategories() const;

private slots:
    /**
     * @brief resetObject
     */
    void resetObject();

    /**
     * @brief Called after container entry was removed to update model.
     * @param idx Index of removed container entry.
     */
    void onContainerEntryRemoved(int idx);

    /**
     * @brief Called after container entry was added to update model.
     * @param idx Index of added container entry.
     */
    void onContainerEntryAdded(int idx);
};

#endif // GTPROPERTYMODEL_H
