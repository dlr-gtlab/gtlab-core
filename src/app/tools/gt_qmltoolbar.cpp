/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gt_qmltoolbar.h"

#include <QDebug>
#include <QQuickItem>

#include <QLayout>

#include <QQuickWidget>
#include <QQmlContext>

struct GtQmlToolbar::Impl
{
    QVariantListModel m_toolbarGroupsList;
    GtQmlToolbarGroup m_statusActions;
    QQuickWidget qmlToolbar;

    bool m_darkMode = {false};
};

GtQmlToolbar::GtQmlToolbar(QWidget* parent) :
    QWidget(parent), pimpl(std::make_unique<Impl>())
{
    static auto typeRegistered = []() {
        qmlRegisterType<GtQmlAction>("de.gtlab", 2, 1, "ToolbarAction");
        return true;
    }();
    Q_UNUSED(typeRegistered);


    pimpl->qmlToolbar.rootContext()->setContextProperty("toolbar", this);
    pimpl->qmlToolbar.setSource(QUrl(QStringLiteral("qrc:/qml/Toolbar.qml")));

    auto layout = new QVBoxLayout();
    layout->addWidget(&pimpl->qmlToolbar);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    pimpl->qmlToolbar.setResizeMode(QQuickWidget::SizeRootObjectToView);
}

GtQmlToolbar::~GtQmlToolbar() = default;

QVariantListModel*
GtQmlToolbar::toolbarGroups()
{
    return &pimpl->m_toolbarGroupsList;
}

void
GtQmlToolbar::addToolbarGroup(GtQmlToolbarGroup* group)
{

    if (!group)
        return;

    pimpl->m_toolbarGroupsList.append(QVariant::fromValue(group));
    emit groupsChanged();
}

void
GtQmlToolbar::removeToolbarGroup(const QString& groupId)
{
    pimpl->m_toolbarGroupsList.removeItem(1);
    emit groupsChanged();

    return;
}

void
GtQmlToolbar::addStatusAction(GtQmlAction* action)
{
    pimpl->m_statusActions.append(action);
    emit statusActionsChanged();
}

bool
GtQmlToolbar::darkmode() const
{
    return pimpl->m_darkMode;
}

void
GtQmlToolbar::setDarkmode(bool d)
{
    if (d == pimpl->m_darkMode)
        return;

    pimpl->m_darkMode = d;
    emit darkmodeChanged();
}

QVariantListModel*
GtQmlToolbar::groups()
{
    return &pimpl->m_toolbarGroupsList;
}

GtQmlToolbarGroup*
GtQmlToolbar::statusActions()
{
    return &pimpl->m_statusActions;
}
