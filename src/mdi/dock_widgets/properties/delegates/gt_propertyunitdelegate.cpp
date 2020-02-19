/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QComboBox>
#include <QApplication>
#include <QPainter>

#include "gt_propertyunitdelegate.h"
#include "gt_propertymodel.h"
#include "gt_abstractproperty.h"
#include "gt_unitconverter.h"

GtPropertyUnitDelegate::GtPropertyUnitDelegate(QObject* parent) :
    QStyledItemDelegate(parent)
{

}

QWidget*
GtPropertyUnitDelegate::createEditor(QWidget* parent,
                                     const QStyleOptionViewItem& option,
                                     const QModelIndex& index) const
{
    m_comboBox = new QComboBox(parent);

    QVariant var = index.data(GtPropertyModel::UnitCategoryRole);

    GtUnit::Category cat = var.value<GtUnit::Category>();

    GtUnitConverter<double>* unitConverter =
        &GtUnitConverter<double>::instance();

    QStringList units = unitConverter->units(cat);

    foreach (const QString& unit, units)
    {
        m_comboBox->addItem(unit);
    }

    connect(m_comboBox, SIGNAL(currentIndexChanged(int)),
            SLOT(setData(int)));

    m_comboBox->setMinimumSize(sizeHint(option, index));

    m_width = sizeHint(option, index).width();
    m_heigth = sizeHint(option, index).height();

    m_comboBox->setMinimumContentsLength(3);

    m_comboBox->setSizeAdjustPolicy(
                QComboBox::AdjustToContents);

    return m_comboBox;
}

void
GtPropertyUnitDelegate::setEditorData(QWidget* editor,
                                      const QModelIndex& index) const
{
    QComboBox* comboBox = static_cast<QComboBox*>(editor);

    if (comboBox == Q_NULLPTR)
    {
        return;
    }
    GtUnitConverter<double>* unitConverter =
        &GtUnitConverter<double>::instance();

    QString unit = index.data(Qt::EditRole).toString();

    QVariant var = index.data(GtPropertyModel::UnitCategoryRole);

    GtUnit::Category cat = var.value<GtUnit::Category>();

    QStringList units = unitConverter->units(cat);
    int value = units.indexOf(unit);

    comboBox->setCurrentIndex(value);

    comboBox->setMinimumWidth(m_width);
    comboBox->setMaximumHeight(m_heigth);
}

void
GtPropertyUnitDelegate::setModelData(QWidget* editor,
                                     QAbstractItemModel* model,
                                     const QModelIndex& index) const
{
    QComboBox* comboBox = static_cast<QComboBox*>(editor);

    if (comboBox == Q_NULLPTR)
    {
        return;
    }

    QString val = comboBox->currentText();

    model->setData(index, val, Qt::EditRole);

    comboBox->setMinimumWidth(m_width);
    comboBox->setMaximumHeight(m_heigth);
}

void
GtPropertyUnitDelegate::updateEditorGeometry(QWidget* editor,
        const QStyleOptionViewItem& option,
        const QModelIndex& /*index*/) const
{
    if (editor == Q_NULLPTR)
    {
        return;
    }
    editor->setGeometry(option.rect);
    editor->setMinimumWidth(m_width);
    editor->setMaximumHeight(m_heigth);
    m_width = editor->width();
    m_heigth = editor->height();
}

void
GtPropertyUnitDelegate::setData(int /*val*/)
{
    emit commitData(m_comboBox);
}

void
GtPropertyUnitDelegate::paint(QPainter* painter,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const
{
    bool isCategory = index.data(GtPropertyModel::CategoryRole).toBool();

    if (painter == Q_NULLPTR)
    {
        return;
    }

    if (isCategory)
    {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    painter->save();

    QStyledItemDelegate::paint(painter, option, index);

    QColor color =
        static_cast<QRgb>(QApplication::style()->styleHint(
                              QStyle::SH_Table_GridLineColor, &option));
    painter->setPen(QPen(color));

    int right = (option.direction == Qt::LeftToRight) ? option.rect.right() :
                 option.rect.left();
    painter->drawLine(right, option.rect.y(), right, option.rect.bottom());

    painter->restore();
}