/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 * Author: Martin Siggel (AT-TWK)
 */

#include "gt_qmlaction.h"

struct GtQmlAction::Impl
{
    QUrl m_iconSource;
    QString m_text = {"Action"};
    QString m_tooltip;
    bool m_enabled = {true};
    bool m_visible = {true};
    bool m_isSeparator = {false};
};

GtQmlAction::~GtQmlAction() = default;

GtQmlAction::GtQmlAction(QObject *parent) : QObject(parent)
{}

GtQmlAction::GtQmlAction(QString text, QUrl icon, QObject *parent)
    : QObject(parent), pimpl(std::make_unique<Impl>())
{
    pimpl->m_iconSource = std::move(icon);
    pimpl->m_text = std::move(text);
}

GtQmlAction*
GtQmlAction::makeSeparator(QObject *parent)
{
    auto obj = new GtQmlAction("", QUrl(), parent);
    obj->pimpl->m_isSeparator = true;
    return obj;
}

QUrl
GtQmlAction::iconUrl() const
{
    return pimpl->m_iconSource;
}

void
GtQmlAction::setIconUrl(const QUrl& url)
{
    if (pimpl->m_iconSource == url) return;

    pimpl->m_iconSource = url;
    emit iconUrlChanged();
}

QString
GtQmlAction::text() const
{
    return pimpl->m_text;
}

void
GtQmlAction::setText(const QString& text)
{
    if (pimpl->m_text == text) return;

    pimpl->m_text = text;
    emit textChanged();
}

QString
GtQmlAction::toolTip() const
{
    return pimpl->m_tooltip;
}

void
GtQmlAction::setToolTip(QString t)
{
    if (t == pimpl->m_tooltip) return;

    pimpl->m_tooltip = std::move(t);
    emit toolTipChanged();
}

bool
GtQmlAction::enabled() const
{
    return pimpl->m_enabled;
}

void
GtQmlAction::setEnabled(bool enabled)
{
    if (pimpl->m_enabled == enabled) return;

    pimpl->m_enabled = enabled;
    emit enabledChanged();
}

bool
GtQmlAction::isVisible() const
{
    return pimpl->m_visible;
}

void
GtQmlAction::setVisible(bool visible)
{
    if (pimpl->m_visible == visible) return;

    pimpl->m_visible = visible;
    emit visibleChanged();
}


bool
GtQmlAction::isSeparator() const
{
    return pimpl->m_isSeparator;
}

