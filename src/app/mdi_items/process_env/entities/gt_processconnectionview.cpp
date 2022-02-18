/* GTlab - Gas Turbine laboratory
 * Source File: gt_processconnectionview.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QPainter>
#include <QApplication>
#include <QDebug>

#include "gt_processconnectionmodel.h"
#include "gt_processconnectiongraphicsview.h"

#include "gt_processconnectionview.h"

GtProcessConnectionView::GtProcessConnectionView(QWidget* parent) :
    GtTreeView(parent),
    m_graphicsView(Q_NULLPTR)
{

}

void
GtProcessConnectionView::setGraphicsView(
        GtProcessConnectionGraphicsView* graphicsView)
{
    m_graphicsView = graphicsView;
}

QModelIndex
GtProcessConnectionView::lastVisibleIndex(const QModelIndex& index)
{
    // check model
    if (model() == Q_NULLPTR)
    {
        return {};
    }

    // get row count for given index
    const int rowCount = model()->rowCount(index);

    // check row count
    if (rowCount > 0)
    {
        // Find the last item in this level of hierarchy.
        QModelIndex lastIndex = model()->index(rowCount - 1, 0, index);

        // check last index
        if (model()->hasChildren(lastIndex) && isExpanded(lastIndex))
        {
            // There is even deeper hierarchy. Drill down with recursion.
            return lastVisibleIndex(lastIndex);
        }

        // Test the last item in the tree.
        return lastIndex;

    }

    // return invalid index
    return {};
}

QModelIndex
GtProcessConnectionView::lastVisibleParentIndex(GtProcessConnectionItem* item)
{
    // check item
    if (item == nullptr)
    {
        return {};
    }

    GtProcessConnectionModel* conMod = connectionModel();

    // check model
    if (conMod == nullptr)
    {
        return {};
    }

    QModelIndex index = conMod->indexFromItem(item);

    return lastVisibleParentIndex(index);



//    if (rowCount > 0)
//    {
//        // Find the last item in this level of hierarchy.
//        QModelIndex lastIndex = model->index(rowCount - 1, 0, index);
//        if (model->hasChildren(lastIndex) && view->isExpanded(lastIndex))
//        {
//            // There is even deeper hierarchy. Drill down with recursion.
//            return isItemVisible(view, testItem, lastIndex);
//        }
//        else
//        {
//            // Test the last item in the tree.
//            return (lastIndex == testItem);
//        }
//    }
//    else
//    {
//        return false;
//    }
}

QModelIndex
GtProcessConnectionView::lastVisibleParentIndex(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return {};
    }

    if (!isExpanded(index))
    {
        QModelIndex tmpIndex = lastVisibleParentIndex(index.parent());

        if (tmpIndex.isValid())
        {
            return tmpIndex;
        }

        return index;
    }

    QModelIndex parentIndex = index.parent();

    if (!parentIndex.isValid())
    {
        return {};
    }

    return lastVisibleParentIndex(parentIndex);

//    qDebug() << "#### get last visible parent index";
//    qDebug() << "   |-> index " << index;
//    qDebug() << "   |-> row count = " << rowCount;
//    qDebug() << "   |-> isExpanded = " << isExpanded(index);
}

GtProcessConnectionModel*
GtProcessConnectionView::connectionModel()
{
    return qobject_cast<GtProcessConnectionModel*>(model());
}

GtProcessConnectionItem*
GtProcessConnectionView::itemById(const QString& uuid, const QString& propId)
{
    // get connection model
    GtProcessConnectionModel* connModel = connectionModel();

    // check connection model
    if (connModel == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    return connModel->itemById(uuid, propId);
}

void GtProcessConnectionView::drawRow(QPainter* painter,
                                      const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const
{
    QStyleOptionViewItemV3 opt = option;

    // check whether index has property role
    //bool isProperty =
    //        index.data(GtProcessConnectionModel::PropertyRole).toBool();


    GtTreeView::drawRow(painter, opt, index);

    QColor color = static_cast<QRgb>(QApplication::style()->styleHint(
                                         QStyle::SH_Table_GridLineColor, &opt));
    painter->save();
    painter->setPen(QPen(color));
    painter->drawLine(opt.rect.x(), opt.rect.bottom(),
                      opt.rect.right(), opt.rect.bottom());
    painter->restore();
}

void
GtProcessConnectionView::paintEvent(QPaintEvent* event)
{
    // check process connection graphics view
    if (m_graphicsView != Q_NULLPTR)
    {
        // update ports
        m_graphicsView->updatePorts(this, event);
    }

    GtTreeView::paintEvent(event);
}
