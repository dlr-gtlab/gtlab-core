/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 02.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */
#ifndef GTPROPERTYCONTROLSCHEDULEITEM_H
#define GTPROPERTYCONTROLSCHEDULEITEM_H

#include "gt_propertyitem.h"

class GtControlScheduleProperty;

class GtPropertyControlScheduleItem : public GtPropertyItem
{
    Q_OBJECT

public:
    Q_INVOKABLE GtPropertyControlScheduleItem();

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
    GtControlScheduleProperty* csProperty() const;

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

#endif // GTPROPERTYCONTROLSCHEDULEITEM_H
