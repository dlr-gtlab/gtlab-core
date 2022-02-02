/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyaccessselectionitem.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 01.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_PROPERTYACCESSSELECTIONITEM_H
#define GT_PROPERTYACCESSSELECTIONITEM_H

#include "gt_propertyitem.h"

class GtAccessSelectionProperty;

/**
 * @brief The GtPropertyAccessSelectionItem class
 */
class GtPropertyAccessSelectionItem : public GtPropertyItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE GtPropertyAccessSelectionItem();

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
                         const QVariant &value,
                         GtObject* obj,
                         int role = Qt::EditRole) override;

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

    /**
     * @brief paint
     * @param painter
     * @param option
     * @return
     */
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option) const override;

    /**
     * @brief modeProperty
     * @return
     */
    GtAccessSelectionProperty* selectionProperty() const;

    /**
     * @brief accessDataHosts
     * @return
     */
    QStringList accessDataHosts() const;

};

#endif // GT_PROPERTYACCESSSELECTIONITEM_H
