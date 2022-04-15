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

#include "gt_gui_exports.h"

#include "gt_shortcutsettingsdata.h"

#include <QDockWidget>

class GtProject;

/**
 * @brief The GtDockWidget class
 */
class GT_GUI_EXPORT GtDockWidget : public QDockWidget
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

    /**
     * @brief registerShortCut
     * @param id - id of the shortcut to use
     * @param k - key sequence to use
     * @param readOnly - optional flag to define it as read only
     * @return the key sequence used for this id
     */
    QKeySequence registerShortCut(QString const& id,
                                  QKeySequence const& k,
                                  bool readOnly = false);

    /**
     * @brief registerShortCut
     * @param data - short cut data to register
     * @return the key sequence used for this id
     */
    QKeySequence registerShortCut(GtShortCutSettingsData const& data);

    /**
     * @brief registerShortCuts
     * @param list - list of short cuts to register
     */
    void registerShortCuts(QList<GtShortCutSettingsData> const& list);

    /**
     * @brief getShortCut
     * @param id - identification string of the short cut to read
     * @return short cut for this id registered in this object
     */
    QKeySequence getShortCut(const QString& id);

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
