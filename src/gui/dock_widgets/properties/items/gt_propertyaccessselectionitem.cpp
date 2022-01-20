/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyaccessselectionitem.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 01.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QComboBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>

#include "gt_accessselectionproperty.h"
#include "gt_propertyvaluedelegate.h"
#include "gt_accessgroup.h"
#include "gt_accessdata.h"
#include "gt_accessmanager.h"

#include "gt_propertyaccessselectionitem.h"

GtPropertyAccessSelectionItem::GtPropertyAccessSelectionItem()
{

}

QVariant
GtPropertyAccessSelectionItem::data(int column, int role) const
{
    if (column < 0 || column >= 3)
    {
        return QVariant();
    }

    if (m_property == Q_NULLPTR)
    {
        return QVariant();
    }

    if (column == 0)
    {
        return GtPropertyItem::data(column, role);
    }

    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            if (column == 2)
            {
                if (selectionProperty() != Q_NULLPTR)
                {
                    if (selectionProperty()->getVal().isEmpty())
                    {
                        return QStringLiteral("<< select >>");
                    }

                    return selectionProperty()->getVal();
                }
            }
        }
    }

    return GtPropertyItem::data(column, role);
}

bool
GtPropertyAccessSelectionItem::setData(int column, const QVariant& value,
                                       GtObject* obj, int role)
{
    if (selectionProperty() == Q_NULLPTR)
    {
        return false;
    }

    if (column == 0)
    {
        GtPropertyItem::setData(column, value, obj, role);
        return true;
    }

    if (column != 2)
    {
        return false;
    }

    if (role != Qt::EditRole)
    {
        return false;
    }

    if (!accessDataHosts().contains(value.toString()))
    {
        return false;
    }

    return GtPropertyItem::setData(column, value, obj, role);
}

QWidget*
GtPropertyAccessSelectionItem::editorWidget(
        QWidget* parent, const GtPropertyValueDelegate* delegate) const
{
    QComboBox* box = new QComboBox(parent);

    box->addItem(QStringLiteral("<< select >>"));

    if (selectionProperty() != Q_NULLPTR)
    {
        box->addItems(accessDataHosts());
    }

    connect(box, SIGNAL(currentIndexChanged(int)),
            delegate, SLOT(onDataChange()));

    return box;
}

void
GtPropertyAccessSelectionItem::setEditorData(QWidget* editor,
                                             QVariant& /*var*/) const
{
    if (selectionProperty() == Q_NULLPTR)
    {
        return;
    }

    QComboBox* box = static_cast<QComboBox*>(editor);

    if (selectionProperty()->get().isEmpty())
    {
        box->setCurrentIndex(0);
    }
    else
    {
        box->setCurrentText(selectionProperty()->get());
    }
}

void
GtPropertyAccessSelectionItem::setModelData(QWidget* editor,
                                            QAbstractItemModel* model,
                                            const QModelIndex& index) const
{
    if (selectionProperty() == Q_NULLPTR)
    {
        return;
    }

    QComboBox* box = static_cast<QComboBox*>(editor);

    model->setData(index, box->currentText(), Qt::EditRole);
}

void
GtPropertyAccessSelectionItem::paint(QPainter* painter,
                                     const QStyleOptionViewItem& option) const
{
    if (selectionProperty() == Q_NULLPTR)
    {
        return;
    }

    painter->save();
    QStyleOptionComboBox box;
    box.rect = option.rect;
    //    box.currentText = selectionProperty()->get();

    //    if (box.currentText.isEmpty())
    //    {
    //        box.currentText = QStringLiteral("<< select >>");
    //    }

    QStringList entries = accessDataHosts();

    if (!entries.isEmpty())
    {
        box.state = QStyle::State_Active | QStyle::State_Enabled;
    }

    QRect r = QApplication::style()->subControlRect(QStyle::CC_ComboBox,
              &box,
              QStyle::SC_ComboBoxArrow);

    QStyleOptionViewItem opt = option;

    double dx = (r.width() - 10) / 2;
    double dy = (r.height() - 10) / 2;

    opt.rect = QRect(r.x() + dx, r.y() + dy, 10, 10);

    QApplication::style()->drawPrimitive(QStyle::PE_IndicatorArrowDown,
                                         &opt, painter);

    painter->restore();
}

GtAccessSelectionProperty*
GtPropertyAccessSelectionItem::selectionProperty() const
{
    return qobject_cast<GtAccessSelectionProperty*>(m_property);
}

QStringList
GtPropertyAccessSelectionItem::accessDataHosts() const
{
    if (selectionProperty() == Q_NULLPTR)
    {
        return QStringList();
    }

    GtAccessGroup* group =
        gtAccessManager->accessGroup(selectionProperty()->accessId());

    if (group == Q_NULLPTR)
    {
        return QStringList();
    }

    QStringList retval;

    foreach (GtAccessData data, group->accessData())
    {
        retval << data.host();
    }

    return retval;
}
