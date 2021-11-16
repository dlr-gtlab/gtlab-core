/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMAINWIN_H
#define GTMAINWIN_H

#include "gt_gui_exports.h"

#include <QMainWindow>
#include <QMap>
#include <QPointer>

namespace Ui
{
class GtMainWin;
}

class QSignalMapper;
class GtProject;
class GtCornerWidget;
class GtAbstractLoadingHelper;
class GtObject;
class GtDockWidget;
class GtProcessQueueWidget;
class QUndoView;

/**
 * @brief The GtMainWin class
 */
class GT_GUI_EXPORT GtMainWin : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief GtMainWin
     * @param parent
     */
    GtMainWin(QWidget* parent = 0);

    /**
     * @brief ~GtMainWin
     */
    virtual ~GtMainWin();

protected:
    /** Called after window is opened
        @param show event */
    virtual void showEvent(QShowEvent* event);

    /**
     * @brief closeEvent
     * @param event
     */
    virtual void closeEvent(QCloseEvent* event);

    /**
     * @brief keyPressEvent
     * @param event
     */
    virtual void keyPressEvent(QKeyEvent* event);

private:
    /// UI Form
    Ui::GtMainWin* ui;

    /// Session mapper
    QSignalMapper* m_switchSessionMapper;

    /// Perspectrive mapper
    QSignalMapper* m_switchPerspectiveMapper;

    /// Corner widget
    GtCornerWidget* m_cornerWidget;

    /// Force quit indicator
    bool m_forceQuit;

    /// First time show event indicator
    bool m_firstTimeShowEvent;

    /// List of initialized dock widgets
    QMap<GtDockWidget*, QAction*> m_dockWidgets;

    /// Pointer to Process Queue Widget
    QPointer<GtProcessQueueWidget> m_processQueue;

    /// Pointer to Undo View Widget
    QPointer<QUndoView> m_undoView;

    /**
     * @brief setupDockWidgets
     */
    void setupDockWidgets();

    /**
     * @brief updateCollectionEntries
     */
    void updateCollectionEntries();

    /**
     * @brief checkForUpdate
     */
    void checkForUpdate();

    /**
     * @brief widgetStructureHelper
     * @param wid
     * @param ident
     */
    void widgetStructureHelper(QWidget* wid, int indent);

    /**
     * @brief setTheme
     * @param dark - true for dark mode, false for default
     */
    void setTheme(bool dark);

private slots:
    /**
     * @brief showProjectWizard
     */
    void showProjectWizard();

    /**
     * @brief importProject
     */
    void importProject();

    /**
     * @brief showPreferences
     */
    void showPreferences();

    /**
     * @brief showSessionPreferences
     */
    void showSessionPreferences();

    /**
     * @brief showPerspectivePreferences
     */
    void showPerspectivePreferences();

    /**
     * @brief updateSessionList
     */
    void updateSessionList();

    /**
     * @brief updatePerspectiveList
     */
    void updatePerspectiveList();

    /**
     * @brief openMapEditor
     */
    void openMapEditor();

    /**
     * @brief openMementoViewer
     */
    void openMementoViewer();

    /**
     * @brief Just a test method
     */
    void openPreDesignPlot();

    /**
     * @brief openSessionViewer
     */
    void openSessionViewer();

    /**
     * @brief opens the state viewer widget
     */
    void openStateViewer();

    /**
     * @brief Just a test method
     */
    void openModelEditor();

    /**
     * @brief Open Widget in the Mdi-Area for examples
     */
    void openExamplesWidget();

    /**
     * @brief opens the command history widget
     */
    void openCommandHistory();

    /**
     * @brief opens the process queue widget
     */
    void openProcessQueue();

    /**
     * @brief onSessionAction
     * @param widget
     */
    void onSessionAction(QObject* widget);

    /**
     * @brief onPerspectiveAction
     * @param widget
     */
    void onPerspectiveAction(QObject* widget);

    /**
     * @brief perspectiveChanged
     * @param id
     */
    void loadPerspectiveSettings(const QString& id = QString());

    /**
     * @brief savePerspectiveSettings
     */
    void savePerspectiveSettings();

    /**
     * @brief Saves current project.
     */
    void saveCurrentProject();

    /**
     * @brief Saves current project under user defined directory.
     */
    void saveCurrentProjectAs();

    /**
     * @brief Duplicates current project.
     */
    void duplicateCurrentProject();

    /**
     * @brief saveAllProjects
     */
    void saveAllProjects();

    /**
     * @brief Called after current project changed to set up ui items.
     */
    void onCurrentProjectChanged(GtProject* project);

    /**
     * @brief printCurrentMdiItem
     */
    void printCurrentMdiItem();

    /**
     * @brief runBugReport
     */
    void runBugReport();

    /**
     * @brief openHelpContents
     */
    void openHelpContents();

    /**
     * @brief openAboutDialog
     */
    void openAboutDialog();

    /**
     * @brief openChangelogDialog
     */
    void openChangelogDialog();

    /**
     * @brief updateWindowTitle
     */
    void updateWindowTitle();

    /**
     * @brief onCollectionEntryClicked
     */
    void onCollectionEntryClicked();

    /**
     * @brief openCheckForUpdatesDialog
     */
    void openCheckForUpdatesDialog();

    /**
     * @brief updateAvailable
     */
    void updateAvailable();

    /**
     * @brief noUpdateAvailable
     * @param str
     */
    void noUpdateAvailable(int errorCode, const QString& str);

    /**
     * @brief runLoadingProcedure
     */
    void runLoadingProcedure(GtAbstractLoadingHelper* helper);

    /**
     * @brief runUpdate
     */
    void runUpdate();

    /**
     * @brief initAfterStartup
     */
    void initAfterStartup();

    /**
     * @brief Opens currently selected project. Does nothing if current project
     * cannot be opened.
     */
    void openProject();

    /**
     * @brief Closes currently selected project. Does nothing if current project
     * cannot be closed.
     */
    void closeProject();

    /**
     * @brief Called after object is selected.
     * @param obj Currently selected object.
     */
    void onObjectSelected(GtObject* obj);

    /**
     * @brief Called on dock widget visibility change to change menu action
     * checked state.
     * @param val New dock widget visibility.
     */
    void onDockVisibilityChange(bool val);

    /**
     * @brief Called after one dock widget action was clicked to show or hide
     * the corresponding dock widget.
     */
    void onDockActionClicked();

    /**
     * @brief onWidgetStructureClicked
     */
    void onWidgetStructureClicked();

    /**
     * @brief onChangeThemeClicked
     */
    void onChangeThemeClicked();

signals:
    /**
     * @brief Emitted after gui is finally initialized.
     */
    void guiInitialized();

};

#endif // GTMAINWIN_H
