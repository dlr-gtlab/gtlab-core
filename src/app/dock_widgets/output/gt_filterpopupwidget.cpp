/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_filterpopupwidget.h"

#include "gt_filteredlogmodel.h"
#include "gt_searchwidget.h"

#include <QCheckBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollArea>
#include <QLabel>

namespace {
    gt::LogLevelFlags levelFlagsFromSet(const QSet<int>& gtLogLevelSet)
    {
        gt::LogLevelFlags levels;
        levels.setFlag(gt::TraceLevelFlag, gtLogLevelSet.contains(gt::log::TraceLevel));
        levels.setFlag(gt::DebugLevelFlag, gtLogLevelSet.contains(gt::log::DebugLevel));
        levels.setFlag(gt::InfoLevelFlag, gtLogLevelSet.contains(gt::log::InfoLevel));
        levels.setFlag(gt::WarningLevelFlag, gtLogLevelSet.contains(gt::log::WarningLevel));
        levels.setFlag(gt::ErrorLevelFlag, gtLogLevelSet.contains(gt::log::ErrorLevel));
        levels.setFlag(gt::FatalLevelFlag, gtLogLevelSet.contains(gt::log::FatalLevel));
        return levels;
    }

    QSet<int> gtLogLevelSetByFlags(gt::LogLevelFlags flags)
    {
        QSet<int> retVal;

        if (flags.testFlag(gt::TraceLevelFlag)) retVal.insert(gt::log::TraceLevel);
        if (flags.testFlag(gt::DebugLevelFlag)) retVal.insert(gt::log::DebugLevel);
        if (flags.testFlag(gt::InfoLevelFlag)) retVal.insert(gt::log::InfoLevel);
        if (flags.testFlag(gt::WarningLevelFlag)) retVal.insert(gt::log::WarningLevel);
        if (flags.testFlag(gt::ErrorLevelFlag)) retVal.insert(gt::log::ErrorLevel);
        if (flags.testFlag(gt::FatalLevelFlag)) retVal.insert(gt::log::FatalLevel);

        return retVal;
    }
}


gt::FilterPopupWidget::FilterPopupWidget(QWidget* parent) :
    QWidget(parent, Qt::Popup)
{
    setFocusPolicy(Qt::StrongFocus);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);
    
    m_searchWidget = new GtSearchWidget(this);
    m_searchWidget->hide();
    
    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    auto* contentWidget = new QWidget(this);
    auto* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(2);
    contentLayout->setAlignment(Qt::AlignTop);
    
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
    
    // Set maximum height to show only 5 items
    int itemHeight = QFontMetrics(font()).height();
    int maxVisibleItems = 7;
    scrollArea->setMaximumHeight(itemHeight * maxVisibleItems + 10);
    
    m_contentWidget = contentWidget;
    m_contentLayout = contentLayout;
    
    m_mainLayout = mainLayout;
    m_scrollArea = scrollArea;
    
    auto* buttonBar = new QWidget(this);
    auto* buttonLayout = new QHBoxLayout(buttonBar);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(5);
    
    auto* selectAll = new QPushButton(tr("All"), buttonBar);
    selectAll->setMaximumWidth(50);
    selectAll->setToolTip(tr("Select all"));
    
    auto* selectNone = new QPushButton(tr("None"), buttonBar);
    selectNone->setMaximumWidth(50);
    selectNone->setToolTip(tr("Select none"));
    
    buttonLayout->addWidget(selectAll);
    buttonLayout->addWidget(selectNone);
    buttonLayout->addStretch(1);
    
    mainLayout->addWidget(buttonBar);
    
    connect(selectAll, &QPushButton::clicked, this, [this](){
        m_updating = true;
        for (QCheckBox* cb : m_checkBoxes)
        {
            cb->setChecked(true);
        }
        m_updating = false;
        updateSelection();
    });
    
    connect(selectNone, &QPushButton::clicked, this, [this](){
        m_updating = true;
        for (QCheckBox* cb : m_checkBoxes)
        {
            cb->setChecked(false);
        }
        m_updating = false;
        updateSelection();
    });
}

void
gt::FilterPopupWidget::setSearchMode()
{
    if (m_contentWidget)
    {
        m_contentWidget->hide();
    }
    
    if (m_searchWidget)
    {
        m_searchWidget->show();
        m_searchWidget->enableSearch();
    }
    
    QList<QPushButton*> buttons = findChildren<QPushButton*>();
    for (QPushButton* btn : buttons)
    {
        btn->hide();
    }
    
    if (m_scrollArea)
    {
        m_scrollArea->hide();
    }
    
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    adjustSize();
    show();
}

void
gt::FilterPopupWidget::setSearchWidget(GtSearchWidget* searchWidget)
{
    if (m_contentWidget)
    {
        m_contentWidget->hide();
    }

    if (m_searchWidget && m_mainLayout)
    {
        m_mainLayout->removeWidget(m_searchWidget);
        m_searchWidget->hide();
    }
    
    m_searchWidget = searchWidget;
    if (searchWidget && m_mainLayout)
    {
        auto* hL = new QHBoxLayout;
        hL->addWidget(new QLabel(tr("Filter:")));
        hL->addWidget(searchWidget);

        m_mainLayout->addLayout(hL);
        searchWidget->show();
        connect(searchWidget, &GtSearchWidget::textChanged,
                this, &FilterPopupWidget::searchTextChanged);

    }
}

void
gt::FilterPopupWidget::setLevelItems(const QStringList& items,
                                     const QList<int>& values,
                                     gt::LogLevelFlags selected)
{
    if (items.size() != values.size()) return;
    
    m_itemToInt.clear();
    for (int i = 0; i < items.size(); ++i)
    {
        m_itemToInt[items[i]] = values[i];
    }
    
    createCheckBoxesForLevels(items);
    
    QSet<int> selectedSet = gtLogLevelSetByFlags(selected);

    m_updating = true;
    for (QCheckBox* cb : m_checkBoxes)
    {
        int value = m_itemToInt.value(cb->text(), -1);
        cb->setChecked(selectedSet.contains(value));
    }
    m_updating = false;
}

void
gt::FilterPopupWidget::setCategoryItems(const QStringList& displayItems,
                                        const QStringList& storageItems,
                                        const QSet<QString>& selectedStorageValues)
{
    if (displayItems.size() != storageItems.size()) return;

    m_itemToInt.clear();
    m_displayToStorage.clear();
    
    for (int i = 0; i < displayItems.size(); ++i)
    {
        QString display = displayItems[i];
        QString storage = storageItems[i];
        
        m_displayToStorage[display] = storage;
    }
    
    createCheckBoxesForCategories(displayItems, storageItems);
    
    m_updating = true;
    for (QCheckBox* cb : m_checkBoxes)
    {
        QString storageValue = m_displayToStorage.value(cb->text());
        cb->setChecked(selectedStorageValues.contains(storageValue));
    }
    m_updating = false;
}

void
gt::FilterPopupWidget::createCheckBoxesForLevels(const QStringList& items)
{
    if (!m_contentWidget || !m_contentLayout) return;
    
    qDeleteAll(m_checkBoxes);
    m_checkBoxes.clear();
    
    for (const QString& item : items)
    {
        QCheckBox* cb = new QCheckBox(item, m_contentWidget);
        cb->setTristate(false);
        m_contentLayout->addWidget(cb);
        
        connect(cb, &QCheckBox::toggled, this, [this, cb](){
            if (!m_updating)
            {
                updateSelection();
            }
        });
        
        m_checkBoxes.append(cb);
    }
}

void
gt::FilterPopupWidget::createCheckBoxesForCategories(
    const QStringList& displayItems, const QStringList& storageItems)
{
    if (!m_contentWidget || !m_contentLayout) return;
    
    qDeleteAll(m_checkBoxes);
    m_checkBoxes.clear();
    
    for (int i = 0; i < displayItems.size(); ++i)
    {
        QString display = displayItems[i];
        QString storage = storageItems[i];
        
        QCheckBox* cb = new QCheckBox(display, m_contentWidget);
        cb->setTristate(false);
        
        // EmptyID in italic font
        if (display == gt::EmptyIDText)
        {
            QFont italicFont = cb->font();
            italicFont.setItalic(true);
            cb->setFont(italicFont);
        }
        
        m_contentLayout->addWidget(cb);
        
        connect(cb, &QCheckBox::toggled, this, [this, cb](){
            if (!m_updating)
            {
                updateSelection();
            }
        });
        
        m_checkBoxes.append(cb);
    }
}

void
gt::FilterPopupWidget::updateSelection()
{
    if (m_itemToInt.isEmpty() && m_displayToStorage.isEmpty())
    {
        QSet<QString> selected;
        for (QCheckBox* cb : m_checkBoxes)
        {
            if (cb->isChecked())
            {
                selected.insert(cb->text());
            }
        }
        emit selectionChanged(selected);
    }
    else if (!m_itemToInt.isEmpty())
    {
        QSet<int> selected;
        for (QCheckBox* cb : m_checkBoxes)
        {
            if (cb->isChecked())
            {
                int value = m_itemToInt.value(cb->text(), -1);
                if (value >= 0)
                {
                    selected.insert(value);
                }
            }
        }
        emit selectionChangedLevel(levelFlagsFromSet(selected));
    }
    else
    {
        QSet<QString> selected;
        for (QCheckBox* cb : m_checkBoxes)
        {
            if (cb->isChecked())
            {
                QString storageValue = m_displayToStorage.value(cb->text());
                if (m_displayToStorage.contains(cb->text()))
                {
                    selected.insert(storageValue);
                }
            }
        }
        emit selectionChangedCategory(selected);
    }
}
