/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyobjectlinkitem.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 15.02.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYOBJECTLINKITEM_H
#define GTPROPERTYOBJECTLINKITEM_H

#include "gt_propertyitem.h"

class GtObjectLinkProperty;

/**
 * @brief The GtPropertyObjectLinkItem class
 */
class GtPropertyObjectLinkItem : public GtPropertyItem
{
    Q_OBJECT

public:
    /**
     * @brief GtPropertyObjectLinkItem
     * @param scope
     * @param property
     * @param model
     * @param parent
     */
    Q_INVOKABLE GtPropertyObjectLinkItem();

    /**
     * @brief data
     * @param column
     * @param role
     * @return
     */
    virtual QVariant data(int column, int role) const;

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
    GtObjectLinkProperty* objectLinkProperty() const;

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

};

#endif // GTPROPERTYOBJECTLINKITEM_H
