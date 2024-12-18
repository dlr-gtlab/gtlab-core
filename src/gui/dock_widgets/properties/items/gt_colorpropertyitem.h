/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.12.2024
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#ifndef GTPROPERTYTURBOMAPITEM_H
#define GTPROPERTYTURBOMAPITEM_H

#include "gt_propertyitem.h"

class GtColorProperty;

/**
 * @brief The GtColorPropertyItem class
 */
class GtColorPropertyItem : public GtPropertyItem
{
    Q_OBJECT

public:
    /**
      * @brief Constructor.
      */
    Q_INVOKABLE GtColorPropertyItem();

    /**
     * @brief data
     * @param column
     * @param role
     * @return
     */
    virtual QVariant data(int column, int role) const override;

    /**
     * @brief setData
     * @param column
     * @param value
     * @param role
     * @return
     */
    virtual bool setData(int column,
                         const QVariant& value,
                         GtObject* obj,
                         int role = Qt::EditRole) override;

    /**
     * @brief colorProperty
     * @return
     */
    GtColorProperty* colorProperty() const;

    /**
     * @brief editorWidget
     * @return
     */
    virtual QWidget* editorWidget(
        QWidget* parent,
        const GtPropertyValueDelegate* delegate) const override;

    /**
     * @brief setEditorData
     * @param var
     */
    virtual void setEditorData(QWidget* editor,
                               QVariant& var) const override;

    /**
     * @brief setModelData
     * @param editor
     * @param model
     */
    virtual void setModelData(QWidget* editor,
                              QAbstractItemModel* model,
                              const QModelIndex& index) const override;

};

#endif // GTPROPERTYTURBOMAPITEM_H
