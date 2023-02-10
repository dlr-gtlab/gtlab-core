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

#include <memory>

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
class GtQmlToolbar;
class QUndoView;
class QQuickWidget;
class QMdiSubWindow;

/**
 * @brief The GtMainWin class
 */
class GtMainWin : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief GtMainWin
     * @param parent
     */
    explicit GtMainWin(QWidget* parent = nullptr);

    /**
     * @brief ~GtMainWin
     */
    ~GtMainWin() override;

public slots:
    /**
     * @brief opens the process queue widget
     */
    void openProcessQueue();

protected:
    /** Called after window is opened
        @param show event */
    void showEvent(QShowEvent* event) override;

    /**
     * @brief closeEvent
     * @param event
     */
    void closeEvent(QCloseEvent* event) override;

    /**
     * @brief keyPressEvent
     * @param event
     */
    void keyPressEvent(QKeyEvent* event) override;

    /**
     * @brief resizeEvent
     * @param event
     */
    void resizeEvent(QResizeEvent* event) override;

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

    /// Qml toolbar widget
    QPointer<GtQmlToolbar> m_mainWindowToolbar;


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

private slots:

    /**
     * @brief setTheme
     * @param dark - true for dark mode, false for default
     */
    void setTheme(bool dark);

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
     * @brief Opens the preferences dialog on the page with the given title.
     * @param title Title of the page to be displayed first.
     */
    void showPreferences(const QString& title);

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
     * @brief Open Widget in the Mdi-Area for examples
     */
    void openExamplesWidget();

    /**
     * @brief opens the command history widget
     */
    void openCommandHistory();

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
     * @brief openAboutDialog
     */
    void openAboutModulesDialog();

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
     * @brief Called by activate an mdi item.
     * @param mdi sub window index
     */
    void onEditorWindowActive(int editorIndex);

signals:
    /**
     * @brief Emitted after gui is finally initialized.
     */
    void guiInitialized();

    /**
     * @brief Emitted after project was opened or closed.
     */
    void projectOpened(bool);

    /**
     * @brief Emitted if current selected mdi item is printable
     */
    void currentMdiItemPrintable(bool);

};

#endif // GTMAINWIN_H
