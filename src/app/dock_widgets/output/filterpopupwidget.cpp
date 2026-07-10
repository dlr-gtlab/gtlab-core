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

FilterPopupWidget::FilterPopupWidget(QWidget* parent) :
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

void FilterPopupWidget::setItems(const QStringList& items, const QSet<QString>& selected)
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

void FilterPopupWidget::setItems(const QStringList& items, const QList<int>& values,
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

void FilterPopupWidget::createCheckBoxes(const QStringList& items)
{
    QVBoxLayout* contentLayout = nullptr;
    QWidget* contentWidget = nullptr;
    
    for (int i = 0; i < this->layout()->count(); ++i)
    {
        if (QWidget* w = qobject_cast<QWidget*>(this->layout()->itemAt(i)->widget()))
        {
            if (w->layout())
            {
                if (w->layout()->count() > 0)
                {
                    if (QScrollArea* sa = qobject_cast<QScrollArea*>(w->layout()->itemAt(0)->widget()))
                    {
                        if (sa->widget())
                        {
                            contentWidget = sa->widget();
                            break;
                        }
                    }
                }
            }
        }
    }
    
    if (!contentWidget)
    {
        return;
    }
    
    if (QVBoxLayout* l = qobject_cast<QVBoxLayout*>(contentWidget->layout()))
    {
        contentLayout = l;
    }
    
    if (!contentLayout)
    {
        return;
    }
    
    qDeleteAll(m_checkBoxes);
    m_checkBoxes.clear();
    
    for (const QString& item : items)
    {
        QCheckBox* cb = new QCheckBox(item, contentWidget);
        cb->setTristate(false);
        contentLayout->addWidget(cb);
        
        connect(cb, &QCheckBox::toggled, this, [this, cb](){
            if (!m_updating)
            {
                updateSelection();
            }
        });
        
        m_checkBoxes.append(cb);
    }
}

void FilterPopupWidget::updateSelection()
{
    if (m_itemToInt.isEmpty())
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
    else
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
}

QSet<QString> FilterPopupWidget::selectedValues() const
{
    QSet<QString> selected;
    for (QCheckBox* cb : m_checkBoxes)
    {
        if (cb->isChecked())
        {
            selected.insert(cb->text());
        }
    }
    return selected;
}

QSet<int> FilterPopupWidget::selectedIntValues() const
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
