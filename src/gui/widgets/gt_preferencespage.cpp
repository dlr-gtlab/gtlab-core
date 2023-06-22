/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QLabel>
#include <QFont>

#include "gt_preferencespage.h"
#include "gt_icons.h"

GtPreferencesPage::GtPreferencesPage(const QString& title, QWidget* parent) :
    QWidget(parent),
    m_title(title),
    m_icon(gt::gui::icon::application())
{
}

void
GtPreferencesPage::setTitle(const QString& id)
{
    m_title = id;
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

QString
GtPreferencesPage::title() const
{
    return m_title;
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

