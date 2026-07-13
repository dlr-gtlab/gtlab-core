/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "filterpopupwidget.h"

#include <QCheckBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QApplication>

gt::FilterPopupWidget::FilterPopupWidget(QWidget* parent) :
    QWidget(parent, Qt::Popup)
{
    setFocusPolicy(Qt::StrongFocus);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);// | Qt::WindowStaysInTopHint);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);
    
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    QWidget* contentWidget = new QWidget(this);
    QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(2);
    contentLayout->setAlignment(Qt::AlignTop);
    
    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
    
    // Set maximum height to show only 5 items
    int itemHeight = QFontMetrics(font()).height();
    int maxVisibleItems = 5;
    scrollArea->setMaximumHeight(itemHeight * maxVisibleItems + 10);
    
    m_contentWidget = contentWidget;
    m_contentLayout = contentLayout;
    
    QWidget* buttonBar = new QWidget(this);
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonBar);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(5);
    
    QPushButton* selectAll = new QPushButton(tr("All"), buttonBar);
    selectAll->setMaximumWidth(50);
    selectAll->setToolTip(tr("Select all"));
    
    QPushButton* selectNone = new QPushButton(tr("None"), buttonBar);
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
gt::FilterPopupWidget::setItems(const QStringList& items,
                                const QSet<QString>& selected)
{
    m_itemToInt.clear();
    for (int i = 0; i < items.size(); ++i)
    {
        m_itemToInt[items[i]] = i;
    }
    
    createCheckBoxes(items);
    
    m_updating = true;
    for (QCheckBox* cb : m_checkBoxes)
    {
        cb->setChecked(selected.contains(cb->text()));
    }
    m_updating = false;
}

void
gt::FilterPopupWidget::setItems(const QStringList& items,
                                const QList<int>& values,
                                const QSet<int>& selected)
{
    if (items.size() != values.size())
    {
        return;
    }
    
    m_itemToInt.clear();
    for (int i = 0; i < items.size(); ++i)
    {
        m_itemToInt[items[i]] = values[i];
    }
    
    createCheckBoxes(items);
    
    m_updating = true;
    for (QCheckBox* cb : m_checkBoxes)
    {
        int value = m_itemToInt.value(cb->text(), -1);
        cb->setChecked(selected.contains(value));
    }
    m_updating = false;
}

void
gt::FilterPopupWidget::setItems(const QStringList& displayItems,
                                const QStringList& storageItems,
                                const QSet<QString>& selectedStorageValues)
{
    if (displayItems.size() != storageItems.size())
    {
        return;
    }
    
    m_itemToInt.clear();
    m_displayToStorage.clear();
    m_storageToDisplay.clear();
    
    for (int i = 0; i < displayItems.size(); ++i)
    {
        QString display = displayItems[i];
        QString storage = storageItems[i];
        
        m_displayToStorage[display] = storage;
        m_storageToDisplay[storage] = display;
    }
    
    createCheckBoxesForStrings(displayItems, storageItems);
    
    m_updating = true;
    for (QCheckBox* cb : m_checkBoxes)
    {
        QString storageValue = m_displayToStorage.value(cb->text());
        cb->setChecked(selectedStorageValues.contains(storageValue));
    }
    m_updating = false;
}

void
gt::FilterPopupWidget::createCheckBoxes(const QStringList& items)
{
    if (!m_contentWidget || !m_contentLayout)
    {
        return;
    }
    
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
gt::FilterPopupWidget::createCheckBoxesForStrings(const QStringList& displayItems,
                                                  const QStringList& storageItems)
{
    if (!m_contentWidget || !m_contentLayout)
    {
        return;
    }
    
    qDeleteAll(m_checkBoxes);
    m_checkBoxes.clear();
    
    for (int i = 0; i < displayItems.size(); ++i)
    {
        QString display = displayItems[i];
        QString storage = storageItems[i];
        
        QCheckBox* cb = new QCheckBox(display, m_contentWidget);
        cb->setTristate(false);
        
        // EmptyID in italic font
        if (display == "EmptyID")
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
        emit selectionChangedInt(selected);
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
        emit selectionChangedStorage(selected);
    }
}

QSet<QString>
gt::FilterPopupWidget::selectedValues() const
{
    QSet<QString> selected;
    for (QCheckBox* cb : m_checkBoxes)
    {
        if (cb->isChecked())
        {
            // if mapping exists, return display value
            if (m_displayToStorage.contains(cb->text()))
            {
                selected.insert(cb->text());
            }
            // Else: direct text
            else
            {
                selected.insert(cb->text());
            }
        }
    }
    return selected;
}

QSet<int>
gt::FilterPopupWidget::selectedIntValues() const
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

    return selected;
}

QSet<QString>
gt::FilterPopupWidget::selectedStorageValues() const
{
    QSet<QString> selected;
    for (QCheckBox* cb : m_checkBoxes)
    {
        if (cb->isChecked())
        {
            QString storageValue = m_displayToStorage.value(cb->text());
            // If mapping existsts, use storage value
            if (m_displayToStorage.contains(cb->text()))
            {
                selected.insert(storageValue);
            }
        }
    }
    return selected;
}
