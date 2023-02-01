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

namespace gt
{
    QString propertyItemCommandString(const QString& objName,
                                      const QString& propertyName,
                                      const QString& commandStr);

    bool propertyItemChange(GtObject& obj,
                            GtAbstractProperty& property,
                            const QVariant& value,
                            const QString& unit);
}

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
                 const QVariant& value,
                 GtObject* obj,
                 int role = Qt::EditRole) override;

    /**
     * @brief editorWidget
     * @return
     */
    QWidget* editorWidget(QWidget* parent,
        const GtPropertyValueDelegate* delegate) const override;

    /**
     * @brief setEditorData
     * @param editor
     * @param var
     * @param scope
     */
    void setEditorData(QWidget* editor,
                       QVariant& var) const override;

    /**
     * @brief setModelData
     * @param editor
     * @param model
     */
    void setModelData(QWidget* editor,
                      QAbstractItemModel* model,
                      const QModelIndex& index) const override;

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
