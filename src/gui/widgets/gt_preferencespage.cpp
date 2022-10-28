/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QDebug>

#include "gt_preferencespage.h"
#include "gt_icons.h"

GtPreferencesPage::GtPreferencesPage(const QString& title, QWidget* parent) :
    QWidget(parent)
{
    m_layout = new QVBoxLayout;
    m_title = new QLabel(title);
    m_icon = gt::gui::icon::application();

    QFont font = m_title->font();
    font.setBold(true);
    m_title->setFont(font);

    m_layout->addWidget(m_title);
    m_layout->addSpacing(20);

    setLayout(m_layout);
}

void
GtPreferencesPage::setTitle(const QString& id)
{
    m_title->setText(id);
}

void
GtPreferencesPage::setTitleShort(const QString &shortTitle)
{
    m_shortTitle = shortTitle;
}

void
GtPreferencesPage::setIcon(const QIcon &icon)
{
    m_icon = icon;
}

void
GtPreferencesPage::addStretch(int val)
{
    m_layout->addStretch(val);
}

QString
GtPreferencesPage::title() const
{
    return m_title->text();
}

QString
GtPreferencesPage::titleShort() const
{
    return m_shortTitle.isEmpty() ? title() : m_shortTitle;
}

QIcon
GtPreferencesPage::icon() const
{
    return m_icon;
}

