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
#include <QDebug>

#include "gt_propertyvaluedelegate.h"
#include "gt_propertytreeview.h"
#include "gt_abstractpropertyitem.h"
#include "gt_project.h"

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

        return item->editorWidget(parent, this);
    }

    return Q_NULLPTR;
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

//    QVariant var = index.data();

//    switch (var.type())
//    {
//    case QVariant::StringList:
//    {
////        QStyleOptionComboBox* box = new QStyleOptionComboBox();
////        box->palette = option.palette;
////        box->rect = option.rect;
////        box->state = QStyle::State_Active | QStyle::State_Enabled;
////        box->currentText = index.data(Qt::EditRole).toString();
////        box->frame = false;
////        box->
////        painter->save();
////        //        QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, box, painter);
////        //        painter->restore();
////        QStyleOptionViewItemV4 opt = option;
////        initStyleOption(&opt, index);

////        const QWidget *widget = opt.widget;
////        QStyle *style = widget ? widget->style() : QApplication::style();
//////        style->drawComplexControl(QStyle::CC_ComboBox, box, painter);
////        style->drawControl(QStyle::CE_ComboBoxLabel, box, painter);
////        painter->restore();

//        QStyleOptionProgressBar progressBarOption;
//        progressBarOption.rect = option.rect;
//        progressBarOption.minimum = 0;
//        progressBarOption.maximum = 100;
//        progressBarOption.progress = 90;
//        progressBarOption.text = QString::number(90) + "%";
//        progressBarOption.textVisible = true;

//        QApplication::style()->drawControl(QStyle::CE_ProgressBar,
//                                           &progressBarOption, painter);

//        painter->save();
//        QStyleOptionComboBox box;
//        box.rect = option.rect;
//        box.currentText = index.data(Qt::EditRole).toString();
////        QStyleOptionViewItemV4 opt = option;
////        initStyleOption(&opt, index);
//        if (!index.data().toStringList().isEmpty())
//        {
//            box.state = QStyle::State_Active | QStyle::State_Enabled;
//        }

//        QStyledItemDelegate::paint(painter, option, index);

////        QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &box,
////                                                  painter);

//        QStringList entries = var.toStringList();
//        if (entries.size() > 1)
//        {
//            QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel,
//                                               &box, painter);
//        }

////        QStyle::PrimitiveElement arrow = QStyle::PE_IndicatorArrowRight;
////        drawPrimitive(arrow, option, painter);
////        box.su
//        QRect r = QApplication::style()->subControlRect(QStyle::CC_ComboBox,
//                                               &box, QStyle::SC_ComboBoxArrow);

//        QStyleOptionViewItem opt = option;

//        double dx = (r.width() - 10) / 2;
//        double dy = (r.height() - 10) / 2;
////        r.setWidth(10);
////        r.setHeight(10);
//        opt.rect = QRect(r.x() + dx, r.y() + dy, 10, 10);

////        qDebug() << r;
//        QApplication::style()->drawPrimitive(QStyle::PE_IndicatorArrowDown,
//                                             &opt, painter);
////        QApplication::style()->drawComplexControl(QStyle::SC_ComboBoxArrow,
////                                           &box, painter);

////        QRect r(option.rect.x(), )
////        painter->fillRect();
//        painter->restore();

//        return;
//    }
//    }

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
