/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTDOCKWIDGET_H
#define GTDOCKWIDGET_H

#include "gt_mdi_exports.h"

#include <QDockWidget>

class GtProject;

/**
 * @brief The GtDockWidget class
 */
class GT_MDI_EXPORT GtDockWidget : public QDockWidget
{
    Q_OBJECT

public:
    /** Returns the recommended dock widget area in main windows.
        @return dock widget area */
    virtual Qt::DockWidgetArea getDockWidgetArea() = 0;

    /** Returns icon which is displayed in the titlebar
        @return titlebar icon */
    virtual QIcon getIcon() const;

    /**
     * @brief Virtual function to add functionality after application
     * startup was successful
     */
    virtual void initAfterStartup();

protected:
    /**
     * @brief GtDockWidget
     */
    Q_INVOKABLE GtDockWidget();

    /** Event called after current project has changed.
        @param new current project */
    virtual void projectChangedEvent(GtProject* project);

    /** Event called after project is closed.
        @param changed project */
    virtual void projectClosedEvent(GtProject* project);

    /**
     * @brief Event called after project was saved.
     * @param project Savend project.
     */
    virtual void projectSavedEvent(GtProject* project);

private slots:
    /** Called after current project has changed.
        @param project new current project */
    void onProjectChanged(GtProject* project);

    /** Called after project emitted close status.
        @param project changed project */
    void onProjectClosed(GtProject* project);

    /**
     * @brief Called after project was saved.
     * @param project Saved project.
     */
    void onProjectSaved(GtProject* project);

};

#endif // GTDOCKWIDGET_H
