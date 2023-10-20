/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 05.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTPROPERTYITEM_H
#define GTABSTRACTPROPERTYITEM_H

#include "gt_gui_exports.h"

#include <QPointer>

#include "gt_object.h"

class GtPropertyModel;
class QAbstractItemModel;
class QPainter;
class QStyleOptionViewItem;
class QMimeData;
class GtPropertyValueDelegate;
class GtObject;
class GtPropertyItemFactory;

class GT_GUI_EXPORT GtAbstractPropertyItem : public GtObject
{
    Q_OBJECT

    friend class GtPropertyItemFactory;
    friend class GtPropertyModel;

public:
    /**
     * @brief data
     * @param column
     * @param role
     * @return
     */
    virtual QVariant data(int column, int role) const = 0;

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
                         int role = Qt::EditRole) = 0;

    /**
     * @brief editorWidget
     * @return
     */
    virtual QWidget* editorWidget(QWidget* parent,
                                 const GtPropertyValueDelegate* delegate) const;

    /**
     * @brief setEditorData
     * @param var
     */
    virtual void setEditorData(QWidget* editor, QVariant& var) const;

    /**
     * @brief setModelData
     * @param editor
     * @param model
     */
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model,
                              const QModelIndex& index) const;

    /**
     * @brief paint
     * @param painter
     * @param option
     * @return
     */
    virtual void paint(QPainter* painter,
                       const QStyleOptionViewItem& option) const;

    /**
     * @brief scope
     * @return
     */
    GtObject* scope() const;

    /**
     * @brief setScope
     * @param obj
     */
    void setScope(GtObject* obj);

    /**
     * @brief isOptional
     * @return
     */
    bool isOptional();

    /**
     * @brief acceptDrop
     * Overwrite, if the property item should accept some mime data
     * to be dropped
     * @param data - mime data to check
     * @return false by default
     */
    virtual bool acceptDrop(const QMimeData* data) const;

    /**
     * @brief dropMimeData
     * Overwrite, if the property item should react when some mime data is dropped
     * does nothing by default
     *
     * If this function should be used the acceptDrop function
     * has to be overwritten as well
     *
     * @param data - mime data to check
     * @return false by default
     */
    virtual bool dropMimeData(const QMimeData* data);

protected:
    /// Property model.
    GtPropertyModel* m_model;

    /// Scope of data representation.
    QPointer<GtObject> m_scope;

    /// Pointer to datamodel property object
    QPointer<GtAbstractProperty> m_property;

    /**
     * @brief Constructor.
     */
    GtAbstractPropertyItem();

    /**
     * @brief setModel
     * @param model
     */
    void setModel(GtPropertyModel* model);

    /**
     * @brief setPropertyData
     * @param prop
     */
    void setPropertyData(GtAbstractProperty* prop);

    /**
     * @brief  update properties (taken only sub-properties into account,
     * that are not hidden)
     */
    void updateSubProperties();


private slots:
    /**
     * @brief Called on property change.
     */
    void onPropertyChange();

};

#endif // GTABSTRACTPROPERTYITEM_H
