/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 29.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPainter>
#include <QApplication>

#include "gt_propertyvaluedelegate.h"
#include "gt_propertytreeview.h"
#include "gt_abstractpropertyitem.h"

GtPropertyValueDelegate::GtPropertyValueDelegate(GtPropertyTreeView* parent) :
    QStyledItemDelegate(parent),
    m_view(parent)
{

}

QWidget*
GtPropertyValueDelegate::createEditor(QWidget* parent,
                                      const QStyleOptionViewItem& /*option*/,
                                      const QModelIndex& index) const
{
    QModelIndex srcIndex = m_view->mapToSource(index);

    if (srcIndex.isValid())
    {
        GtAbstractPropertyItem* item =
               static_cast<GtAbstractPropertyItem*>(srcIndex.internalPointer());

        QWidget* w = item->editorWidget(parent, this);
        w->setAutoFillBackground(true);
        return w;
    }

    return nullptr;
}

void
GtPropertyValueDelegate::setEditorData(QWidget* editor,
                                       const QModelIndex& index) const
{
    QVariant var = index.data();

    QModelIndex srcIndex = m_view->mapToSource(index);

    if (srcIndex.isValid())
    {
        GtAbstractPropertyItem* item =
               static_cast<GtAbstractPropertyItem*>(srcIndex.internalPointer());

        item->setEditorData(editor, var);
    }
}

void
GtPropertyValueDelegate::setModelData(QWidget* editor,
                                      QAbstractItemModel* model,
                                      const QModelIndex& index) const
{
    QModelIndex srcIndex = m_view->mapToSource(index);

    if (srcIndex.isValid())
    {
        GtAbstractPropertyItem* item =
               static_cast<GtAbstractPropertyItem*>(srcIndex.internalPointer());

        item->setModelData(editor, model, index);
    }
}

QString
GtPropertyValueDelegate::displayText(const QVariant& value,
                                     const QLocale& locale) const
{
    if (value.type() == QVariant::Double)
    {
        return value.toString();
    }

    return QStyledItemDelegate::displayText(value, locale);
}

void
GtPropertyValueDelegate::paint(QPainter* painter,
                               const QStyleOptionViewItem& option,
                               const QModelIndex& index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    QModelIndex srcIndex = m_view->mapToSource(index);

    if (srcIndex.isValid())
    {
        GtAbstractPropertyItem* item =
               static_cast<GtAbstractPropertyItem*>(srcIndex.internalPointer());

        item->paint(painter, option);
    }
}

void
GtPropertyValueDelegate::onDataChange()
{
    QWidget* widget = qobject_cast<QWidget*>(sender());
    if (widget)
    {
        emit commitData(widget);
    }
}
