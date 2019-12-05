/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_dockwidget.h"
#include "gt_application.h"
#include "gt_datamodel.h"

GtDockWidget::GtDockWidget()
{
    setObjectName(tr("Dock Widget"));

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

