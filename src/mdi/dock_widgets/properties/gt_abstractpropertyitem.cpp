/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 05.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QLineEdit>
#include <QAbstractItemModel>

#include "gt_project.h"
#include "gt_abstractproperty.h"
#include "gt_propertyitemfactory.h"

#include "gt_abstractpropertyitem.h"

GtAbstractPropertyItem::GtAbstractPropertyItem() :
    m_model(Q_NULLPTR),
    m_scope(Q_NULLPTR),
    m_property(Q_NULLPTR)
{
}

QWidget*
GtAbstractPropertyItem::editorWidget(QWidget* parent,
                              const GtPropertyValueDelegate* /*delegate*/) const
{
    QLineEdit* lineEdit = new QLineEdit(parent);
    return lineEdit;
}

void
GtAbstractPropertyItem::setEditorData(QWidget* editor, QVariant& var) const
{
    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    QString val = var.toString();
    lineEdit->setText(val);
}

void
GtAbstractPropertyItem::setModelData(QWidget* editor,
                                     QAbstractItemModel* model,
                                     const QModelIndex& index) const
{
    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    model->setData(index, lineEdit->text(), Qt::EditRole);
}

void
GtAbstractPropertyItem::paint(QPainter* /*painter*/,
                              const QStyleOptionViewItem& /*option*/) const
{
    // do nothing
}

GtObject*
GtAbstractPropertyItem::scope() const
{
    return m_scope;
}

void
GtAbstractPropertyItem::setScope(GtObject* obj)
{
    m_scope = obj;
    foreach (GtAbstractPropertyItem* item,
             findDirectChildren<GtAbstractPropertyItem*>())
    {
        item->setScope(obj);
    }
}

bool
GtAbstractPropertyItem::isOptional()
{
    if (m_property == Q_NULLPTR)
    {
        return false;
    }

    return m_property->isOptional();
}

void
GtAbstractPropertyItem::setModel(GtPropertyModel* model)
{
    m_model = model;
}

void
GtAbstractPropertyItem::setPropertyData(GtAbstractProperty* prop)
{
    m_property = prop;
    connect(prop, SIGNAL(changed()), SLOT(onPropertyChange()));
}

void
GtAbstractPropertyItem::updateSubProperties()
{
    if (m_property == Q_NULLPTR)
    {
        return;
    }

    if (m_property->properties().isEmpty())
    {
        return;
    }

    foreach (GtAbstractProperty* prop, m_property->properties())
    {
        if (!prop->isHidden())
        {
            gtPropertyItemFactory->newItem(prop, m_scope, m_model, this);
        }
    }
}

void
GtAbstractPropertyItem::onPropertyChange()
{
    emit dataChanged(this, m_property);
}
