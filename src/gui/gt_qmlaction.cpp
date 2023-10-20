/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 * Author: Martin Siggel (AT-TWK)
 */

#include "gt_qmlaction.h"

GtQmlAction::GtQmlAction(QString text, QObject* parent) :
    QObject(parent),
    m_text(std::move(text))
{
}

GtQmlAction::GtQmlAction(QObject *parent) : 
    GtQmlAction("", parent)
{
}

GtQmlAction::GtQmlAction(QString text, QUrl iconUrl, QObject *parent) :
    QObject(parent),
    m_text(std::move(text)),
    m_iconUrl(std::move(iconUrl))
{
}

QString
GtQmlAction::text() const
{
    return m_text;
}

void
GtQmlAction::setText(const QString& text)
{
    if (m_text != text)
    {
        m_text = text;
        emit textChanged();
    }
}

QUrl
GtQmlAction::iconUrl() const
{
    return m_iconUrl;

}

void
GtQmlAction::setIconUrl(const QUrl & url)
{
    if (m_iconUrl != url)
    {
        m_iconUrl = url;
        emit iconUrlChanged();
    }
}

bool
GtQmlAction::enabled() const
{
    return m_enabled;
}

void
GtQmlAction::setEnabled(bool enabled)
{
    if (m_enabled != enabled)
    {
        m_enabled = enabled;
        emit enabledChanged();
    }
}

void
GtQmlAction::trigger()
{
    emit triggered();
}
