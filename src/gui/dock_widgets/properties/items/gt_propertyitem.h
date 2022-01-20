/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 28.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYITEM_H
#define GTPROPERTYITEM_H

#include "gt_gui_exports.h"

#include <QPointer>

#include "gt_abstractpropertyitem.h"

class GtAbstractProperty;

/**
 * @brief The GtPropertyItem class
 */
class GT_GUI_EXPORT GtPropertyItem : public GtAbstractPropertyItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    Q_INVOKABLE GtPropertyItem();

    /**
     * @brief ~GtPropertyItem
     */
    virtual ~GtPropertyItem();

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
     * @brief editorWidget
     * @return
     */
    virtual QWidget* editorWidget(
        QWidget* parent,
        const GtPropertyValueDelegate* delegate) const Q_DECL_OVERRIDE;

    /**
     * @brief setEditorData
     * @param editor
     * @param var
     * @param scope
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

protected:
    /// current unit
    QString m_currentUnit;

private slots:
    /**
     * @brief onPropertyChange
     */
    void onPropertyChange();

};

#endif // GTPROPERTYITEM_H
