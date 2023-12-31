/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QFont>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>

#include "gt_propertymodeitem.h"
#include "gt_modeproperty.h"
#include "gt_modetypeproperty.h"
#include "gt_propertyvaluedelegate.h"


namespace
{
    void setBrief(GtModeProperty& prop, QComboBox& box)
    {
        GtModeTypeProperty* mode = prop.typeProperty(prop.get());
        if (mode) box.setToolTip(mode->brief());
    }
}

GtPropertyModeItem::GtPropertyModeItem()
{
}

QVariant
GtPropertyModeItem::data(int column, int role) const
{
    if (column < 0 || column >= 3)
    {
        return QVariant();
    }

    if (!m_property)
    {
        return QVariant();
    }

    if (column == 0)
    {
        return GtPropertyItem::data(column, role);
    }

    switch (role)
    {
        case Qt::EditRole:
        {
            if (column == 2)
            {
                if (modeProperty())
                {
                    return modeProperty()->getVal();
                }
            }
            break;
        }
        case Qt::ToolTipRole:
        {
            if (column == 2)
            {
                if (modeProperty())
                {
                    GtModeTypeProperty* mode = modeProperty()->typeProperty(
                                modeProperty()->get());

                    if (mode)
                    {
                        return mode->brief();
                    }

                    return modeProperty()->getVal();
                }
            }
        }
    }

    return GtPropertyItem::data(column, role);
}

bool
GtPropertyModeItem::setData(int column, const QVariant& value, GtObject* obj,
                            int role)
{
    if (!modeProperty())
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

    if (!modeProperty()->modeExists(value.toString()))
    {
        return false;
    }

    return GtPropertyItem::setData(column, value, obj, role);
}

QWidget*
GtPropertyModeItem::editorWidget(QWidget* parent,
                                 const GtPropertyValueDelegate* delegate) const
{
    QComboBox* box = new QComboBox(parent);

    if (modeProperty())
    {
        box->addItems(modeProperty()->modes());
    }

    connect(box, SIGNAL(currentIndexChanged(int)),
            delegate, SLOT(onDataChange()));

    return box;
}

void
GtPropertyModeItem::setEditorData(QWidget* editor, QVariant& /*var*/) const
{
    if (!modeProperty())
    {
        return;
    }

    QComboBox* box = static_cast<QComboBox*>(editor);

    box->setCurrentText(modeProperty()->get());
    setBrief(*modeProperty(), *box);
}

void
GtPropertyModeItem::setModelData(QWidget* editor, QAbstractItemModel* model,
                                 const QModelIndex& index) const
{
    if (!modeProperty())
    {
        return;
    }

    QComboBox* box = static_cast<QComboBox*>(editor);

    model->setData(index, box->currentText(), Qt::EditRole);

    // the mode prop now has changed, we need to set the brief
    // text to the combo box delegate
    setBrief(*modeProperty(), *box);
}

void
GtPropertyModeItem::paint(QPainter* painter,
                          const QStyleOptionViewItem& option) const
{
    if (!modeProperty())
    {
        return;
    }

    painter->save();
    QStyleOptionComboBox box;
    box.rect = option.rect;
    box.currentText = modeProperty()->get();

    QStringList entries = modeProperty()->modes();

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

GtModeProperty*
GtPropertyModeItem::modeProperty() const
{
    return qobject_cast<GtModeProperty*>(m_property);
}
