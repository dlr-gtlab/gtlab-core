/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_filterheaderview.h"
#include "gt_logfilterproxymodel.h"
#include "gt_filterpopupwidget.h"
#include "gt_searchwidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QMetaObject>
#include <QFile>
#include <QMenu>
#include <QTextStream>

#include "gt_icons.h"

namespace {

const int FILTER_ICON_SIZE = 12;
const int FILTER_BUTTON_WIDTH = 16;

static QStringList logLevelStrings()
{
    return {QString::fromStdString(gt::log::levelToString(gt::log::TraceLevel)),
            QString::fromStdString(gt::log::levelToString(gt::log::DebugLevel)),
            QString::fromStdString(gt::log::levelToString(gt::log::InfoLevel)),
            QString::fromStdString(gt::log::levelToString(gt::log::WarningLevel)),
            QString::fromStdString(gt::log::levelToString(gt::log::ErrorLevel)),
            QString::fromStdString(gt::log::levelToString(gt::log::FatalLevel))
           };
};

static QList<int> logLevelInts()
{
    return {gt::log::levelToInt(gt::log::TraceLevel),
        gt::log::levelToInt(gt::log::DebugLevel),
        gt::log::levelToInt(gt::log::InfoLevel),
        gt::log::levelToInt(gt::log::WarningLevel),
        gt::log::levelToInt(gt::log::ErrorLevel),
        gt::log::levelToInt(gt::log::FatalLevel)
    };
};

}

gt::FilterHeaderView::FilterHeaderView(Qt::Orientation orientation, QWidget* parent) :
    QHeaderView(orientation, parent)
{
    setMinimumSectionSize(50);
    resizeSection(0, 50);
    resizeSection(2, 70);
    
    setSectionsClickable(true);
    setSectionResizeMode(QHeaderView::Interactive);
    
    setMouseTracking(true);
    
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

void
gt::FilterHeaderView::setFilterModel(gt::LogFilterProxyModel* model)
{
    if (m_filterModel)
    {
        disconnect(m_filterModel, nullptr, this, nullptr);
    }
    
    m_filterModel = model;
    
    if (m_filterModel)
    {
        connect(m_filterModel, &QSortFilterProxyModel::modelReset,
                this, [this](){
            m_levelFilters.clear();
            m_categoryFilters.clear();
            viewport()->update();
        });
        
        connect(m_filterModel, &QSortFilterProxyModel::dataChanged,
                this, [this](const QModelIndex& topLeft,
                             const QModelIndex& bottomRight,
                             const QVector<int>& roles){
            Q_UNUSED(roles);
            if (topLeft.column() == 2)
            {
                m_categoryFilters.clear();
            }
            viewport()->update();
        });
        
        connect(m_filterModel, &QSortFilterProxyModel::modelReset,
                this, [this](){
            m_levelFilters.clear();
            m_categoryFilters.clear();
            viewport()->update();
        });
        
        connect(m_filterModel, &gt::LogFilterProxyModel::levelFilterChanged,
                this, [this](){
            viewport()->update();
        });
        
        connect(m_filterModel, &gt::LogFilterProxyModel::categoryFilterChanged,
                this, [this](){
            viewport()->update();
        });
        
        connect(m_filterModel, &gt::LogFilterProxyModel::filterTextChanged,
                this, [this](){
            viewport()->update();
        });
    }
    
    connect(this, &QHeaderView::sectionResized, this, [this](int logicalIndex,
                                                             int oldSize,
                                                             int newSize){
        if (logicalIndex == 0 || logicalIndex == 2 || logicalIndex == 3)
        {
            if (newSize < 70)
            {
                resizeSection(logicalIndex, 70);
            }
        }
    });
}

void
gt::FilterHeaderView::paintSection(QPainter* painter, const QRect& rect,
                                   int logicalIndex) const
{
    if (!m_filterModel || logicalIndex < 0)
    {
        QHeaderView::paintSection(painter, rect, logicalIndex);
        return;
    }

    QRect filterRect = filterButtonRect(logicalIndex);

    if (filterRect.isEmpty()) return;
    
    // Only show filter icon for Level (column 0), Id (column 2) and Message (column 3)
    if (logicalIndex != 0 && logicalIndex != 2 && logicalIndex != 3)
    {
        QHeaderView::paintSection(painter, rect, logicalIndex);
        return;
    }
    
    // Draw header first
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    
    // Draw filter icon for columns with filter capability (0, 2, 3)
    bool hasActiveFilters =
        m_filterModel && m_filterModel->hasActiveFiltersForColumn(logicalIndex);

    QPixmap pixmap;
    if (hasActiveFilters)
    {
        pixmap = gt::gui::icon::filterActive().pixmap(FILTER_ICON_SIZE,
                                                      FILTER_ICON_SIZE);
    }
    else
    {
        pixmap = gt::gui::icon::filter().pixmap(FILTER_ICON_SIZE,
                                                FILTER_ICON_SIZE);
    }
    
    QRect iconRect = filterRect;
    iconRect.setSize(QSize(FILTER_ICON_SIZE, FILTER_ICON_SIZE));
    iconRect.moveCenter(filterRect.center());
    painter->drawPixmap(iconRect.topLeft(), pixmap);
}

void
gt::FilterHeaderView::mousePressEvent(QMouseEvent* event)
{
    int clickedColumn = logicalIndexAt(event->pos());

    if (event->button() == Qt::RightButton &&
        clickedColumn >= 0 &&
        isFilterButtonClicked(event->pos(), clickedColumn) &&
        (clickedColumn == 0 || clickedColumn == 2 || clickedColumn == 3))
    {
        closePopup();

        QMenu menu(this);
        QAction* clearAction = menu.addAction(tr("Clear Filter"));
        clearAction->setEnabled(
            m_filterModel &&
            m_filterModel->hasActiveFiltersForColumn(clickedColumn));

        if (menu.exec(event->globalPos()) == clearAction)
        {
            clearFilter(clickedColumn);
        }
        return;
    }

    if (event->button() != Qt::LeftButton)
    {
        QHeaderView::mousePressEvent(event);
        return;
    }

    if (clickedColumn < 0)
    {
        QHeaderView::mousePressEvent(event);
        return;
    }
    
    if (isFilterButtonClicked(event->pos(), clickedColumn))
    {
        m_activeColumn = clickedColumn;
        
        if (m_popup)
        {
            closePopup();
        }
        
        emit filterButtonClicked(clickedColumn);
        
        if (clickedColumn == 3 && m_filterModel)
        {
            m_popup = new gt::FilterPopupWidget(this);
            
            GtSearchWidget* searchWidget = new GtSearchWidget(this);
            searchWidget->setText(m_filterModel->filterText());
            
            m_popup->setSearchWidget(searchWidget);
            m_popup->setSearchMode();

            connect(searchWidget, &GtSearchWidget::textChanged,
                    m_filterModel, &gt::LogFilterProxyModel::setFilterText);

            connect(m_filterModel, &gt::LogFilterProxyModel::modelReset,
                    this, [this, searchWidget](){
                        searchWidget->setText(m_filterModel->filterText());
                    });

            connect(m_popup, &gt::FilterPopupWidget::destroyed,
                    this, [this](){
                        m_popup = nullptr;
                    });

            m_popup->setMinimumWidth(300);

            QPoint popupPos =
                mapToGlobal(filterButtonRect(logicalIndexAt(event->pos())).bottomLeft());

            int popupWidth = m_popup->minimumWidth();
            int buttonWidth = FILTER_BUTTON_WIDTH;
            int newX = popupPos.x() - popupWidth + buttonWidth;

            m_popup->move(QPoint(newX, popupPos.y()));
            m_popup->show();
            m_popup->activateWindow();
        }
        else
        {
            QStringList items;
            QSet<int> selectedLevels;
            QSet<QString> selectedCategories;
            QStringList displayItems;
            QStringList storageItems;
            
            if (clickedColumn == 0 && m_filterModel)
            {
                items = logLevelStrings();

                selectedLevels = m_filterModel->levelFilter();
                m_levelFilters[clickedColumn] = selectedLevels;
            }
            else if (clickedColumn == 2 && m_filterModel)
            {
                auto itemsWithStorage = m_filterModel->availableCategoriesWithStorage();

                
                for (const auto& pair : itemsWithStorage)
                {
                    displayItems << pair.first;
                    storageItems << pair.second;
                }
                
                items = displayItems;
                
                selectedCategories = m_filterModel->categoryFilter();
                m_categoryFilters[clickedColumn] = selectedCategories;
            }
            
            if (!items.isEmpty() || clickedColumn == 0 || clickedColumn == 2)
            {
                m_popup = new gt::FilterPopupWidget(this);
                
                if (clickedColumn == 0)
                {
                    m_popup->setItems(items, logLevelInts(), selectedLevels);
                    
                    connect(m_popup, &gt::FilterPopupWidget::selectionChangedInt,
                            this, [this, clickedColumn](const QSet<int>& selected){
                                if (m_filterModel)
                                {
                                    m_filterModel->setLevelFilter(selected);
                                    m_levelFilters[clickedColumn] = selected;
                                }
                            });
                }
                else if (clickedColumn == 2)
                {
                    m_popup->setItems(items, storageItems, selectedCategories);
                    
                    connect(m_popup, &gt::FilterPopupWidget::selectionChangedStorage,
                            this, [this, clickedColumn](const QSet<QString>& selected){
                                if (m_filterModel)
                                {
                                    m_filterModel->setCategoryFilter(selected);
                                    m_categoryFilters[clickedColumn] = selected;
                                }
                            });
                }
                
                connect(m_popup, &gt::FilterPopupWidget::destroyed,
                        this, [this](){
                            m_popup = nullptr;
                        });
                
                QPoint popupPos =
                    mapToGlobal(filterButtonRect(logicalIndexAt(event->pos())).bottomLeft());
                m_popup->move(popupPos);
                m_popup->setMinimumWidth(180);
                m_popup->show();
                m_popup->activateWindow();
            }
        }
    }
    
    QHeaderView::mousePressEvent(event);
}

void
gt::FilterHeaderView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_activeColumn >= 0)
    {
        m_activeColumn = -1;
    }
    
    QHeaderView::mouseReleaseEvent(event);
}

void
gt::FilterHeaderView::closePopup()
{
    if (m_popup)
    {
        m_popup->close();
        m_popup->deleteLater();
        m_popup = nullptr;
    }
}

void
gt::FilterHeaderView::clearFilter(int logicalIndex)
{
    if (!m_filterModel) return;

    if (logicalIndex == 0)
    {
        QSet<int> levels;
        for (int level : logLevelInts())
        {
            levels.insert(level);
        }
        m_filterModel->setLevelFilter(levels);
        m_levelFilters[logicalIndex] = levels;
    }
    else if (logicalIndex == 2)
    {
        QSet<QString> categories;
        for (const auto& category :
             m_filterModel->availableCategoriesWithStorage())
        {
            categories.insert(category.second);
        }
        m_filterModel->setDeactivatedCategories({});
        m_filterModel->setCategoryFilter(categories);
        m_categoryFilters[logicalIndex] = categories;
    }
    else if (logicalIndex == 3)
    {
        m_filterModel->setFilterText({});
    }

    viewport()->update();
}

QRect
gt::FilterHeaderView::filterButtonRect(int logicalIndex) const
{
    int sectionPos = sectionPosition(logicalIndex);
    int sectionSizeVal = sectionSize(logicalIndex);
    
    QRect sectionRect(sectionPos, 0, sectionSizeVal, height());
    
    int rightPos = sectionRect.right();
    int topPos = sectionRect.top();
    int height = sectionRect.height();
    
    return QRect(rightPos - FILTER_BUTTON_WIDTH - 2, topPos + 2,
                 FILTER_BUTTON_WIDTH, height - 4);
}

bool
gt::FilterHeaderView::isFilterButtonClicked(const QPoint& pos,
                                            int logicalIndex) const
{
    QRect filterRect = filterButtonRect(logicalIndex);
    return filterRect.contains(pos);
}
