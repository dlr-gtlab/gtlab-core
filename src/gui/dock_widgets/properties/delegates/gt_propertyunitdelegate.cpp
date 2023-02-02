/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

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
    /// No additional delete is needed as the parent-child structure is used
    m_comboBox = new QComboBox(parent);

    QVariant var = index.data(GtPropertyModel::UnitCategoryRole);

    GtUnit::Category cat = var.value<GtUnit::Category>();

    QStringList units;

    if (GtUnitConverter<double>* unitConverter =
            &GtUnitConverter<double>::instance())
    {
        units = unitConverter->units(cat);
    }

    foreach (const QString& unit, units)
    {
        m_comboBox->addItem(unit);
    }


    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(setData(int)));

    m_comboBox->setMinimumSize(sizeHint(option, index));

    m_width = sizeHint(option, index).width();
    m_heigth = sizeHint(option, index).height();

    m_comboBox->setMinimumContentsLength(3);

    m_comboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    return m_comboBox;
}

void
GtPropertyUnitDelegate::setEditorData(QWidget* editor,
                                      const QModelIndex& index) const
{
    QComboBox* comboBox = static_cast<QComboBox*>(editor);

    if (!comboBox)
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

    if (!comboBox)
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
    if (!editor)
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
