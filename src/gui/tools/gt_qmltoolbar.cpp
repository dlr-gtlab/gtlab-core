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

#include <iterator>

struct GtQmlToolbar::Impl
{
    QVariantListModel m_toolbarGroupsList;
    GtQmlToolbarGroup m_statusActions;
    QQuickWidget qmlToolbar;

    QVariantMap theme;
};

GtQmlToolbar::GtQmlToolbar(QWidget* parent) :
    QWidget(parent), pimpl(std::make_unique<Impl>())
{
    static auto typeRegistered = []() {
        qmlRegisterType<GtQmlAction>("de.gtlab", 2, 1, "ToolbarAction");
        return true;
    }();
    Q_UNUSED(typeRegistered);

    setColorTheme(Theme());

    pimpl->qmlToolbar.rootContext()->setContextProperty("toolbar", this);
    pimpl->qmlToolbar.setSource(QUrl(QStringLiteral("qrc:/qml/Toolbar.qml")));

    auto layout = new QVBoxLayout();
    layout->addWidget(&pimpl->qmlToolbar);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);

    pimpl->qmlToolbar.setResizeMode(QQuickWidget::SizeRootObjectToView);
}

GtQmlToolbar::~GtQmlToolbar() = default;

void
GtQmlToolbar::addToolbarGroup(GtQmlToolbarGroup* group)
{
    if (!group) return;

    pimpl->m_toolbarGroupsList.append(QVariant::fromValue(group));
    emit groupsChanged();
}

void
GtQmlToolbar::removeToolbarGroup(GtQmlToolbarGroup * group)
{
    auto iter = std::find_if(pimpl->m_toolbarGroupsList.begin(),
                 pimpl->m_toolbarGroupsList.end(), [group](const QVariant& v)
    {
        auto grp = v.value<GtQmlToolbarGroup*>();
        if (!grp) return false;

        return grp == group;
    });

    if (iter == pimpl->m_toolbarGroupsList.end()) return; // group not found

    auto idx = std::distance(pimpl->m_toolbarGroupsList.begin(), iter);
    pimpl->m_toolbarGroupsList.removeItem(idx);

    emit groupsChanged();
}

void
GtQmlToolbar::addStatusAction(GtQmlAction* action)
{
    pimpl->m_statusActions.append(action);
    emit statusActionsChanged();
}

void
GtQmlToolbar::setColorTheme(const Theme &colors)
{
    QVariantMap theme;
    theme["baseColor"] = colors.base;
    theme["backgroundColor"] = colors.background;
    theme["hoverColor"] = colors.buttonHover;
    theme["darkMode"] = colors.darkmode;

    pimpl->theme = std::move(theme);

    emit themeChanged();
}

QVariantMap
GtQmlToolbar::themeMap() const
{
    return pimpl->theme;
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

GtQmlToolbar::Theme::Theme()
    : base(241, 241, 241)
    , background(255, 255, 255 )
    , buttonHover(221, 238, 255)
    , darkmode(false)
{
}

