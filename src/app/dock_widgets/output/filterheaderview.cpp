/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "filterheaderview.h"
#include "logfilterproxymodel.h"
#include "filterpopupwidget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QMetaObject>
#include <QFile>
#include <QTextStream>

#include "gt_icons.h"

namespace {

const int FILTER_ICON_SIZE = 16;
const int FILTER_BUTTON_WIDTH = 24;

}

FilterHeaderView::FilterHeaderView(Qt::Orientation orientation, QWidget* parent) :
    QHeaderView(orientation, parent)
{
    setSectionsClickable(true);
    setSectionResizeMode(QHeaderView::Interactive);
    
    setMouseTracking(true);
    
    setAttribute(Qt::WA_OpaquePaintEvent, false);
}

void FilterHeaderView::setFilterModel(LogFilterProxyModel* model)
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
            update();
        });
        
        connect(m_filterModel, &QSortFilterProxyModel::dataChanged,
                this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight,
                            const QVector<int>& roles){
            Q_UNUSED(roles);
            if (topLeft.column() == 2)
            {
                m_categoryFilters.clear();
            }
        });
    }
}

void FilterHeaderView::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
    QFile debugFile("C:/temp/paint_debug.txt");
    debugFile.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&debugFile);
    out << "paintSection called for column" << logicalIndex << ", painter active:" << painter->isActive() << "\n";
    debugFile.close();
    
    if (!m_filterModel || logicalIndex < 0)
    {
        return;
    }
    
    QRect filterRect = filterButtonRect(logicalIndex);
    if (filterRect.isEmpty())
    {
        return;
    }
    
    // Draw icon FIRST (before QHeaderView::paintSection)
    gt::gui::Icon filterIcon = gt::gui::icon::search();
    QIcon icon = filterIcon;
    int targetSize = FILTER_ICON_SIZE;
    QPixmap pixmap = icon.pixmap(targetSize, targetSize);
    
    QRect iconRect = filterRect;
    iconRect.setSize(QSize(FILTER_ICON_SIZE, FILTER_ICON_SIZE));
    iconRect.moveCenter(filterRect.center());
    painter->drawPixmap(iconRect.topLeft(), pixmap);
    
    // Draw a red test rectangle to verify drawing works
    QRect testRect = filterRect;
    testRect.setSize(QSize(10, 10));
    testRect.moveCenter(filterRect.center());
    painter->fillRect(testRect, Qt::red);
    
    painter->save();
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    
    // Re-draw icon after header to ensure it's visible
    gt::gui::Icon filterIcon2 = gt::gui::icon::search();
    QIcon icon2 = filterIcon2;
    int targetSize2 = FILTER_ICON_SIZE;
    QPixmap pixmap2 = icon2.pixmap(targetSize2, targetSize2);
    
    QRect iconRect2 = filterRect;
    iconRect2.setSize(QSize(FILTER_ICON_SIZE, FILTER_ICON_SIZE));
    iconRect2.moveCenter(filterRect.center());
    painter->drawPixmap(iconRect2.topLeft(), pixmap2);
}

void FilterHeaderView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        QHeaderView::mousePressEvent(event);
        return;
    }
    
    int clickedColumn = logicalIndexAt(event->pos());
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
        
        QStringList items;
        QSet<int> selectedLevels;
        QSet<QString> selectedCategories;
        QStringList displayItems;
        QStringList storageItems;
        
        auto logToQString = [](gt::log::Level level){
              return QString::fromStdString(gt::log::levelToString(level));
        };

if (clickedColumn == 0 && m_filterModel)
            {
                items << logToQString(gt::log::TraceLevel)
                      << logToQString(gt::log::DebugLevel)
                      << logToQString(gt::log::InfoLevel)
                      << logToQString(gt::log::WarningLevel)
                      << logToQString(gt::log::ErrorLevel)
                      << logToQString(gt::log::FatalLevel);
                
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
        
        if (!items.isEmpty())
        {
            m_popup = new FilterPopupWidget(this);
            
            if (clickedColumn == 0)
            {
                QList<int> values;
                values << gt::log::levelToInt(gt::log::TraceLevel)
                       << gt::log::levelToInt(gt::log::DebugLevel)
                       << gt::log::levelToInt(gt::log::InfoLevel)
                       << gt::log::levelToInt(gt::log::WarningLevel)
                       << gt::log::levelToInt(gt::log::ErrorLevel)
                       << gt::log::levelToInt(gt::log::FatalLevel);
                m_popup->setItems(items, values, selectedLevels);
                
                connect(m_popup, &FilterPopupWidget::selectionChangedInt,
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
                
                connect(m_popup, &FilterPopupWidget::selectionChangedStorage,
                        this, [this, clickedColumn](const QSet<QString>& selected){
                    if (m_filterModel)
                    {
                        m_filterModel->setCategoryFilter(selected);
                        m_categoryFilters[clickedColumn] = selected;
                    }
                });
            }
            
            connect(m_popup, &FilterPopupWidget::destroyed,
                    this, [this](){
                m_popup = nullptr;
            });
            
            QPoint popupPos = mapToGlobal(filterButtonRect(logicalIndexAt(event->pos())).bottomLeft());
            m_popup->move(popupPos);
            m_popup->setMinimumWidth(180);
            m_popup->show();
            m_popup->activateWindow();
        }
    }
    
    QHeaderView::mousePressEvent(event);
}

void FilterHeaderView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_activeColumn >= 0)
    {
        m_activeColumn = -1;
    }
    
    QHeaderView::mouseReleaseEvent(event);
}

void FilterHeaderView::closePopup()
{
    if (m_popup)
    {
        m_popup->close();
        m_popup->deleteLater();
        m_popup = nullptr;
    }
}

QRect FilterHeaderView::filterButtonRect(int logicalIndex) const
{
    int sectionPos = sectionPosition(logicalIndex);
    int sectionSizeVal = sectionSize(logicalIndex);
    
    QRect sectionRect(sectionPos, 0, sectionSizeVal, height());
    
    int rightPos = sectionRect.right();
    int topPos = sectionRect.top();
    int height = sectionRect.height();
    
    return QRect(rightPos - FILTER_BUTTON_WIDTH - 2, topPos + 2, FILTER_BUTTON_WIDTH, height - 4);
}

bool FilterHeaderView::isFilterButtonClicked(const QPoint& pos, int logicalIndex) const
{
    QRect filterRect = filterButtonRect(logicalIndex);
    return filterRect.contains(pos);
}
