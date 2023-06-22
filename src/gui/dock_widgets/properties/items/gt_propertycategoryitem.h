/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
     * @brief If set to true, category item will treated like a container entry
     * @param isContainer
     */
    void setIsContainer(bool isContainer);

    /**
     * @brief If true, category item is a container entry.
     * @return
     */
    bool isContainer() const;

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
    QVariant data(int column, int role) const override;

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

    /**
     * @brief paint
     * @param painter
     * @param option
     * @return
     */
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option) const override;

private:
    /// Category identification string
    QString m_id;

    /// Expand-Collapse icon
    QIcon m_icon;

    /// Container indicator
    bool m_isContainer{false};

};

#endif // GTPROPERTYCATEGORYITEM_H
