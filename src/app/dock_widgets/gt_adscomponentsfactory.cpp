/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "gt_adscomponentsfactory.h"

#include <qtadvanceddocking-qt5/DockAreaTitleBar.h>
#include <qtadvanceddocking-qt5/DockAreaTabBar.h>
#include <qtadvanceddocking-qt5/DockAreaWidget.h>
#include <gt_application.h>

#include <QQuickWidget>
#include <QQmlContext>

class GtAdsAreaTitleBar : public ads::CDockAreaTitleBar
{
public:
    explicit GtAdsAreaTitleBar(ads::CDockAreaWidget* area) :
        ads::CDockAreaTitleBar(area)
    {
        QQuickWidget* qmlTitleBar = new QQuickWidget(this);
        qmlTitleBar->resize(QSize(50, 25));
        qmlTitleBar->setResizeMode(QQuickWidget::SizeRootObjectToView);
        qmlTitleBar->setAttribute(Qt::WA_AlwaysStackOnTop);
//        qmlTitleBar->rootContext()->setContextProperty("dockwidget", this);
        qmlTitleBar->rootContext()->setContextProperty("gtapp", gtApp);
        qmlTitleBar->setSource(QUrl("qrc:/qml/dockbar.qml"));

        insertWidget(indexOf(tabBar()) + 1, qmlTitleBar);

        connect(area, &ads::CDockAreaWidget::currentChanged,
                this, [qmlTitleBar, area](int){
                    if (auto* dock = area->currentDockWidget())
                    {
                        qmlTitleBar->rootContext()->setContextProperty("dockwidget", dock);
                    }
                });
    }
};

ads::CDockAreaTitleBar*
GtAdsComponentsFactory::createDockAreaTitleBar(ads::CDockAreaWidget* area) const
{
    return new ads::CDockAreaTitleBar(area); //GtAdsAreaTitleBar(area);
}
