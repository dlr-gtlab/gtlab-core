/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYMODEITEM_H
#define GTPROPERTYMODEITEM_H

#include "gt_propertyitem.h"

class GtModeProperty;

/**
 * @brief The GtPropertyModeItem class
 */
class GtPropertyModeItem : public GtPropertyItem
{
    Q_OBJECT

public:
    /**
     * @brief GtPropertyModeItem
     * @param property
     * @param parent
     */
    Q_INVOKABLE GtPropertyModeItem();

    /**
     * @brief data
     * @param column
     * @param role
     * @return
     */
    virtual QVariant data(int column, int role) const Q_DECL_OVERRIDE;

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
                         int role = Qt::EditRole) Q_DECL_OVERRIDE;

    /**
     * @brief modeProperty
     * @return
     */
    GtModeProperty* modeProperty() const;

    /**
     * @brief editorWidget
     * @return
     */
    virtual QWidget* editorWidget(
        QWidget* parent,
        const GtPropertyValueDelegate* delegate) const Q_DECL_OVERRIDE;

    /**
     * @brief setEditorData
     * @param var
     */
    virtual void setEditorData(QWidget* editor,
                               QVariant& var) const Q_DECL_OVERRIDE;

    /**
     * @brief setModelData
     * @param editor
     * @param model
     */
    virtual void setModelData(QWidget* editor,
                              QAbstractItemModel* model,
                              const QModelIndex& index) const Q_DECL_OVERRIDE;

    /**
     * @brief paint
     * @param painter
     * @param option
     * @return
     */
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option) const Q_DECL_OVERRIDE;

};

#endif // GTPROPERTYMODEITEM_H
