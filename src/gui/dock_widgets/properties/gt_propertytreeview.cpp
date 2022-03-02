/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 28.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QApplication>
#include <QPainter>
#include <QHeaderView>
#include <QScrollBar>

#include "gt_propertytreeview.h"
#include "gt_propertymodel.h"
#include "gt_abstractproperty.h"
#include "gt_unitconverter.h"
#include "gt_application.h"
#include "gt_treefiltermodel.h"
#include "gt_propertyiddelegate.h"
#include "gt_propertyvaluedelegate.h"
#include "gt_propertyunitdelegate.h"
#include "gt_project.h"

GtPropertyTreeView::GtPropertyTreeView(GtObject* scope,
                                       QWidget* parent) :
    GtTreeView(parent)
{
    setAlternatingRowColors(true);
    setEditTriggers(QAbstractItemView::AllEditTriggers);
    setRootIsDecorated(false);
    setAnimated(true);

    connect(this, SIGNAL(collapsed(QModelIndex)),
            SLOT(onCollapsed(QModelIndex)));
    connect(this, SIGNAL(expanded(QModelIndex)),
            SLOT(onExpanded(QModelIndex)));

    header()->setDefaultSectionSize(20);

    GtPropertyIdDelegate* idDelegate = new GtPropertyIdDelegate(this);
    m_valDelegate = new GtPropertyValueDelegate(this);
    GtPropertyUnitDelegate* unitDelegate = new GtPropertyUnitDelegate(this);

    setItemDelegateForColumn(0, idDelegate);
    setItemDelegateForColumn(1, unitDelegate);
    setItemDelegateForColumn(2, m_valDelegate);

    m_model = new GtPropertyModel(scope, this);
    m_filterModel = new GtTreeFilterModel(m_model);
    m_filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_filterModel->setSourceModel(m_model);

    setModel(m_filterModel);
}

void
GtPropertyTreeView::resizeColumns()
{
    resizeColumnToContents(0);
    resizeColumnToContents(1);

    setColumnWidth(0, columnWidth(0) + 10);
    setColumnWidth(1, columnWidth(1) + 10);
}

GtTreeFilterModel*
GtPropertyTreeView::filterModel()
{
    return m_filterModel;
}

QModelIndex
GtPropertyTreeView::mapToSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return {};
    }

    if (index.model() != m_filterModel)
    {
        return {};
    }

    return  m_filterModel->mapToSource(index);
}

QModelIndex
GtPropertyTreeView::mapFromSource(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return {};
    }

    if (index.model() != m_model)
    {
        return {};
    }

    return  m_filterModel->mapFromSource(index);
}

void
GtPropertyTreeView::setObject(GtObject* obj, bool processEvents)
{
    if (m_model->object() != obj)
    {
        m_model->setObject(obj);

        if (processEvents)
        {
            /// Fix to handle signals from resetting the model
            QCoreApplication::processEvents();
        }
        
        expandAll();
        resizeColumns();
    }
}

void
GtPropertyTreeView::setScope(GtObject* project)
{
    if (!m_model)
    {
        return;
    }

    m_model->setScope(project);
}

void
GtPropertyTreeView::setCategoryFilter(const QStringList& filter)
{
    if (filter.isEmpty())
    {
        return;
    }

    if (!m_model)
    {
        return;
    }

    m_model->setCategoryFilter(filter);
}

void GtPropertyTreeView::drawRow(QPainter* painter,
                                 const QStyleOptionViewItem& option,
                                 const QModelIndex& index) const
{
    QStyleOptionViewItemV3 opt = option;

    //bool isCategory = index.data(GtPropertyModel::CategoryRole).toBool();

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
GtPropertyTreeView::resizeEvent(QResizeEvent* event)
{
    GtTreeView::resizeEvent(event);
}

void
GtPropertyTreeView::mousePressEvent(QMouseEvent* event)
{

    GtTreeView::mousePressEvent(event);
}

QModelIndex
GtPropertyTreeView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                               Qt::KeyboardModifiers modifiers)
{
    /*
     * TODO: Needs own implementation to automatically select the next
     * editable element when the Tab key is pressed.
     * If the next element is not editable, it is necessary to search
     * for the next editable element.
     */
    return GtTreeView::moveCursor(cursorAction, modifiers);
}

void
GtPropertyTreeView::onCollapsed(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    if (!model())
    {
        return;
    }

    bool isCategory = index.data(GtPropertyModel::CategoryRole).toBool();

    if (isCategory)
    {
        bool isMonitoring =
            index.data(GtPropertyModel::MonitoringRole).toBool();

        if (isMonitoring)
        {
            model()->setData(index, gtApp->icon("arrowrightBlueIcon.png"),
                             Qt::DecorationRole);
        }
        else
        {
            model()->setData(index, gtApp->icon("arrowrightIcon.png"),
                             Qt::DecorationRole);
        }
    }
}

void
GtPropertyTreeView::onExpanded(const QModelIndex& index)
{
    if (!index.isValid())
    {
        return;
    }

    if (!model())
    {
        return;
    }

    bool isCategory = index.data(GtPropertyModel::CategoryRole).toBool();

    if (isCategory)
    {
        bool isMonitoring =
            index.data(GtPropertyModel::MonitoringRole).toBool();

        if (isMonitoring)
        {
            model()->setData(index, gtApp->icon("arrowdownBlueIcon.png"),
                             Qt::DecorationRole);
        }
        else
        {
            model()->setData(index, gtApp->icon("arrowdownIcon.png"),
                             Qt::DecorationRole);
        }
    }
}

