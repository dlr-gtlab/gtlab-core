/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 */

#include "gt_abstractpostwidget.h"
#include "gt_posttemplateitem.h"
#include "gt_abstractchartprovider.h"

#include "gt_icons.h"

#include <QWidget>

#include <gt_logging.h>

GtAbstractPostWidget::GtAbstractPostWidget(QWidget* w) :
    QWidget(w),
    m_data(nullptr),
    m_printable(true)
{
    addConfigAction(tr("Configuration Menu"),
                    QStringLiteral("showConfigurationMenu"))
            .setIcon(gt::gui::icon::config())
            .setVerificationMethod(QStringLiteral("canShowConfigurationMenu"));

    addConfigAction(tr("Hide Widgets"), QStringLiteral("hideAxisLabels"))
            .setIcon(gt::gui::icon::eyeOff())
            .setVerificationMethod(QStringLiteral("canHideAxisLabels"))
            .setVisibilityMethod(QStringLiteral("canHideAxisLabels"));

    addConfigAction(tr("Show Widgets"), QStringLiteral("showAxisLabels"))
            .setIcon(gt::gui::icon::eye())
            .setVerificationMethod(QStringLiteral("canShowAxisLabels"))
            .setVisibilityMethod(QStringLiteral("canShowAxisLabels"));

    addConfigAction(tr("Hide Markers"), QStringLiteral("hideMarkers"))
            .setIcon(gt::gui::icon::eyeOff())
            .setVerificationMethod(QStringLiteral("canHideMarkers"))
            .setVisibilityMethod(QStringLiteral("canHideMarkers"));

    addConfigAction(tr("Show Markers"), QStringLiteral("showMarkers"))
            .setIcon(gt::gui::icon::eye())
            .setVerificationMethod(QStringLiteral("canShowMarkers"))
            .setVisibilityMethod(QStringLiteral("canShowMarkers"));
}

GtActionList
GtAbstractPostWidget::configActions()
{
    return m_actionList;
}

GtPostTemplateItem*
GtAbstractPostWidget::data()
{
    return m_data;
}

void
GtAbstractPostWidget::setData(GtPostTemplateItem* dm)
{
    m_data = dm;
}

GtObjectUIAction&
GtAbstractPostWidget::addConfigAction(const QString& actionText,
                                      const QString& actionMethod)
{
    m_actionList << GtObjectUIAction(actionText, actionMethod);
    return m_actionList.last();
}

GtObjectUIAction&
GtAbstractPostWidget::addConfigAction(const QString& actionText, ActionFunction actionMethod)
{
    m_actionList << GtObjectUIAction(actionText, std::move(actionMethod));
    return m_actionList.last();
}

bool
GtAbstractPostWidget::addConfigAction(const QString& actionText,
                                      const QString& actionMethod,
                                      const QString& actionIcon,
                                      const QString& actionVerification)
{
    if (actionText.isEmpty() || actionIcon.isEmpty() || actionMethod.isEmpty())
    {
        return false;
    }

    m_actionList << GtObjectUIAction(actionText, actionMethod, actionIcon,
                                     actionVerification);

    return true;
}

bool
GtAbstractPostWidget::addConfigAction(const QString& actionText,
                                      const QString& actionMethod,
                                      const QString& actionIcon,
                                      const QString& actionVerification,
                                      const QString& actionVisibility)
{
    if (actionText.isEmpty() || actionIcon.isEmpty() || actionMethod.isEmpty())
    {
        return false;
    }

    m_actionList << GtObjectUIAction(actionText, actionMethod, actionIcon,
                                     actionVerification, actionVisibility);

    return true;
}

bool
GtAbstractPostWidget::hasConfigMenu()
{
    return !m_actionList.isEmpty();
}

bool
GtAbstractPostWidget::isPrintable()
{
    return m_printable;
}


QIcon
GtAbstractPostWidget::icon()
{
    return m_icon;
}

void
GtAbstractPostWidget::setIcon(const QIcon& icon)
{
    m_icon = icon;
}

void
GtAbstractPostWidget::initShowHideMarkers()
{

}

QString
GtAbstractPostWidget::providerName()
{
    return QString{};
}

void
GtAbstractPostWidget::updatePlots()
{
    updatePlot();
}

bool
GtAbstractPostWidget::canShowMarkers(GtObject*)
{
    return false;
}

bool
GtAbstractPostWidget::canHideMarkers(GtObject*)
{
    return false;
}

GtAbstractChartProvider*
GtAbstractPostWidget::chartProvider()
{
    if (!m_data)
    {
        gtWarning() << tr("Data is NULL in Abstract Post Widget");

        return nullptr;
    }

    GtAbstractChartProvider* acp =
            qobject_cast<GtAbstractChartProvider*>(m_data);

    if (!acp)
    {
        gtWarning() << tr("Data cannot be casted into Abstract Chart Provider");

        return nullptr;
    }

    return acp;
}
