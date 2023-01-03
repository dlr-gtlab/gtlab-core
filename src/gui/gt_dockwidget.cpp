/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QQuickWidget>
#include <QQmlContext>

#include "gt_dockwidget.h"
#include "gt_application.h"
#include "gt_datamodel.h"

GtDockWidget::GtDockWidget()
{
    setObjectName(tr("Dock Widget"));

    QQuickWidget* qml_titlebar = new QQuickWidget(this);
    qml_titlebar->resize(QSize(50, 25));
    qml_titlebar->setResizeMode(QQuickWidget::SizeRootObjectToView);
    qml_titlebar->rootContext()->setContextProperty("dockwidget", this);
    qml_titlebar->setSource(QUrl("qrc:/qml/dockbar.qml"));
    setTitleBarWidget(qml_titlebar);

    connect(gtApp, SIGNAL(currentProjectChanged(GtProject*)),
            SLOT(onProjectChanged(GtProject*)));
    connect(gtDataModel, SIGNAL(projectSaved(GtProject*)),
            SLOT(onProjectSaved(GtProject*)));
//    connect(gtApp, SIGNAL(projectClosed(GtdProject*)),
//            SLOT(onProjectClosed(GtdProject*)));
}

QIcon
GtDockWidget::getIcon() const
{
    return QIcon();
}

void
GtDockWidget::initAfterStartup()
{
    // nothing to do here
}

void
GtDockWidget::projectChangedEvent(GtProject* project)
{
    Q_UNUSED(project)
}

void
GtDockWidget::projectClosedEvent(GtProject* project)
{
    Q_UNUSED(project)
}

void
GtDockWidget::projectSavedEvent(GtProject* project)
{
    Q_UNUSED(project)
}

QKeySequence
GtDockWidget::registerShortCut(const QString& id,
                               const QKeySequence& k,
                               bool readOnly)
{
    const QMetaObject* m = metaObject();
    return registerShortCut({id, m->className(), k, readOnly});
}

QKeySequence
GtDockWidget::registerShortCut(const GtShortCutSettingsData& data)
{
    gtApp->extendShortCuts(data);
    return getShortCut(data.id);
}

void
GtDockWidget::registerShortCuts(const QList<GtShortCutSettingsData>& list)
{
    gtApp->extendShortCuts(list);
}

QKeySequence
GtDockWidget::getShortCut(const QString &id)
{
    const QMetaObject* m = metaObject();
    return gtApp->getShortCutSequence(id, m->className());
}

void
GtDockWidget::onProjectChanged(GtProject* project)
{
    projectChangedEvent(project);
}

void
GtDockWidget::onProjectClosed(GtProject* project)
{
    projectClosedEvent(project);
}

void
GtDockWidget::onProjectSaved(GtProject* project)
{
    projectSavedEvent(project);
}

