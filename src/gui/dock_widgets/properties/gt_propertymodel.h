/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYMODEL_H
#define GTPROPERTYMODEL_H

#include <QAbstractItemModel>
#include <QPointer>

class GtObject;
class GtPropertyCategoryItem;
class GtAbstractPropertyItem;
class GtAbstractProperty;

/**
 * @brief The GtPropertyModel class
 */
class GtPropertyModel : public QAbstractItemModel
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
        MonitoringRole
    };

    /**
     * @brief Constructor.
     * @param parent Parent object.
     */
    explicit GtPropertyModel(GtObject* scope, QObject* parent = nullptr);

    /**
     * @brief Destructor.
     */
    virtual ~GtPropertyModel();

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
    virtual QVariant headerData(int section,
                                Qt::Orientation orientation,
                                int role =
                                    Qt::DisplayRole) const override;

    /**
     * @brief setObject
     * @param obj
     */
    void setObject(GtObject* obj);

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

private:
    /// pointer to current object
    QPointer<GtObject> m_obj;

    /// property wrapper items
    QList<GtPropertyCategoryItem*> m_properties;

    /// Scope of data representation
    QPointer<GtObject> m_scope;

    /// Category filter
    QStringList m_catFilter;

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

};

#endif // GTPROPERTYMODEL_H
