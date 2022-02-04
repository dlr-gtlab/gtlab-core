/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertylabelitem.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFont>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>

#include "gt_labelproperty.h"
#include "gt_project.h"
#include "gt_propertyvaluedelegate.h"
#include "gt_application.h"
#include "gt_labeldata.h"

#include "gt_propertylabelitem.h"

GtPropertyLabelItem::GtPropertyLabelItem()
{
}

GtLabelProperty*
GtPropertyLabelItem::labelProperty() const
{
    return qobject_cast<GtLabelProperty*>(m_property);
}

QVariant
GtPropertyLabelItem::data(int column, int role) const
{
    if (labelProperty() == Q_NULLPTR)
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            if (column == 2 && labelProperty()->get().isEmpty())
            {
                return QStringLiteral("-");
            }
        }
    }

    return GtPropertyItem::data(column, role);
}

QWidget*
GtPropertyLabelItem::editorWidget(QWidget* parent,
                                  const GtPropertyValueDelegate* delegate) const
{
    QComboBox* box = new QComboBox(parent);

    box->addItems(labelList());

    connect(box, SIGNAL(currentIndexChanged(int)),
            delegate, SLOT(onDataChange()));

    return box;
}

void
GtPropertyLabelItem::setEditorData(QWidget* editor, QVariant& /*var*/) const
{
    if (labelProperty() == nullptr)
    {
        return;
    }

    QComboBox* box = static_cast<QComboBox*>(editor);

    box->setCurrentText(labelProperty()->get());

}

void
GtPropertyLabelItem::setModelData(QWidget* editor,
                                  QAbstractItemModel* model,
                                  const QModelIndex& index) const
{
    if (labelProperty() == nullptr)
    {
        return;
    }

    QComboBox* box = static_cast<QComboBox*>(editor);

    model->setData(index, box->currentText(), Qt::EditRole);
}

void
GtPropertyLabelItem::paint(QPainter* painter,
                           const QStyleOptionViewItem& option) const
{
    if (labelProperty() == Q_NULLPTR)
    {
        return;
    }

    painter->save();

    QStyleOptionComboBox box;
    box.rect = option.rect;

    QStringList entries = labelList();

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

    opt.rect = QRect(int(r.x() + dx), int(r.y() + dy), 10, 10);

    QApplication::style()->drawPrimitive(QStyle::PE_IndicatorArrowDown,
                                         &opt, painter);

    painter->restore();
}

QStringList
GtPropertyLabelItem::labelList() const
{
    if (m_scope == Q_NULLPTR)
    {
        return QStringList();
    }

    GtProject* project = qobject_cast<GtProject*>(m_scope);

    if (project == Q_NULLPTR)
    {
        return QStringList();
    }

    GtLabelData* labelData = project->labelData();

    if (labelData == Q_NULLPTR)
    {
        return QStringList();
    }

    return labelData->labelIds();
}
