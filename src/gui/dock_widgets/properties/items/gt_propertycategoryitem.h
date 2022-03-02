/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 05.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYCATEGORYITEM_H
#define GTPROPERTYCATEGORYITEM_H

#include <QIcon>

#include "gt_abstractpropertyitem.h"

class GtAbstractProperty;

/**
 * @brief The GtPropertyCategoryItem class
 */
class GtPropertyCategoryItem : public GtAbstractPropertyItem
{
    Q_OBJECT

public:
    /**
     * @brief GtPropertyCategoryItem
     * @param id
     * @param parent
     */
    GtPropertyCategoryItem(GtObject* scope,
                           const QString& id,
                           GtPropertyModel* model,
                           GtObject* parent = nullptr);

    /**
     * @brief addPropertyItem
     * @param item
     */
    void addPropertyItem(GtAbstractProperty* property);

    /**
     * @brief categoryId
     * @return
     */
    const QString& categoryId() const;

    /**
     * @brief data
     * @param column
     * @param role
     * @return
     */
    QVariant data(int column, int role) const;

    /**
     * @brief setData
     * @param column
     * @param value
     * @param role
     * @return
     */
    bool setData(int column,
                 const QVariant &value,
                 GtObject* obj,
                 int role = Qt::EditRole) override;

private:
    /// Category identification string
    QString m_id;

    /// Expand-Collapse icon
    QIcon m_icon;

};

#endif // GTPROPERTYCATEGORYITEM_H
