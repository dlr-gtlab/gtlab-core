/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 07.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTAPPLICATION_H
#define GTAPPLICATION_H

#include "gt_gui_exports.h"

#include "gt_coreapplication.h"

#if defined(gtApp)
#undef gtApp
#endif
#define gtApp (static_cast<GtApplication *>(GtCoreApplication::instance()))

#include <QUndoStack>
#include <QMutex>
#include <QPointer>

#include <memory>

#include "gt_shortcutsettingsdata.h"

class QKeyEvent;
class GtPerspective;
class GtObjectUI;
class GtObject;
class GtGuiModuleLoader;
class GtAbstractProperty;
class GtProcessExecutor;
class GtApplicationPrivate;
class GtShortCuts;

class GT_GUI_EXPORT GtApplication : public GtCoreApplication
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    explicit GtApplication(QCoreApplication* parent,
                           bool devMode = false,
                           AppMode mode = AppMode::Gui);

    /**
     * @brief Destructor.
     */
    ~GtApplication() override;

    /** Creates a new icon from a resource file.
     *  @param iconPath Relative or custom resource path to a icon file.
     * The function is deprecated.
     * Please use the icon function of GtGUI.
     */
    [[deprecated("use gt::gui::icon instead")]]
    static QIcon icon(QString const& iconPath);

    /**
     * @brief initMdiLauncher
     */
    void initMdiLauncher();

    /**
     * @brief loadModules
     */
    void loadModules() override;

    /**
     * @brief perspectiveIds
     * @return
     */
    const QStringList& perspectiveIds();

    /**
     * @brief Returns current perspective id. Empty if no perspective is set.
     * @return perspective identification string
     */
    QString perspectiveId();

    /**
     * @brief initPerspective
     * @param id
     */
    void initPerspective(const QString& id = QString());

    /**
     * @brief switchPerspective
     * @param id
     */
    void switchPerspective(const QString& id);

    /**
     * @brief deletePerspective
     * @param id
     * @return
     */
    bool deletePerspective(const QString& id);

    /**
     * @brief renamePerspective
     * @param oldId
     * @param newId
     * @return
     */
    bool renamePerspective(const QString& oldId, const QString& newId);

    /**
     * @brief newPerspective
     * @param id
     * @return
     */
    bool newPerspective(const QString& id);

    /**
     * @brief duplicatePerspective
     * @param source
     * @param target
     * @return
     */
    bool duplicatePerspective(const QString& source, const QString& target);

    /**
     * @brief savePerspective
     */
    void savePerspectiveData(const QByteArray& geometry,
                             const QByteArray& state);

    /**
     * @brief loadPerspectiveData
     * @return
     */
    QPair<QByteArray, QByteArray> loadPerspectiveData();

    /**
     * @brief initShortCuts - intializaion of the short-cuts based
     * on the settings
     */
    void initShortCuts();

    /**
     * @brief Returns list of all registered user interfaces objects for given
     * class.
     * @param obj Object for retrieving UI classes of the associated class.
     * @return List of all registered user interfaces objects.
     */
    QList<GtObjectUI*> objectUI(GtObject* obj);

    /**
     * @brief Returns list of all registered user interfaces objects for given
     * class name.
     * @param classname Classname for retrieving UI classes.
     * @return List of all registered user interfaces objects.
     */
    QList<GtObjectUI*> objectUI(const QString& classname);

    /**
     * @brief Returns default user interface object. if no user interface object
     * is found, nullptr is returned.
     * @param obj Object for retrieving default UI class of the associated
     * class.
     * @return Default user interface object.
     */
    GtObjectUI* defaultObjectUI(GtObject* obj);

    /**
     * @brief Returns default user interface object. if no user interface object
     * is found, nullptr is returned.
     * @param classname Classname for retrieving default UI class.
     * @return Default user interface object.
     */
    GtObjectUI* defaultObjectUI(const QString& classname);

    /**
     * @brief knownUIObjects - returns a list of all stored object ui classes
     * @return class name list
     */
    QStringList knownUIObjects();

    /**
     * @brief switchSession
     * @param id
     */
    void switchSession(const QString& id) override;

    /**
     * @brief undoStack
     * @return
     */
    QUndoStack* undoStack();

    /**
     * @brief propertyCommand - creates an undo command for a property change
     * event.
     * @param obj
     * @param prop
     * @param newValue
     * @param unit
     * @param root
     */
    void propertyCommand(GtObject* obj,
                         GtAbstractProperty* prop,
                         const QVariant& newValue,
                         const QString& unit = QString(),
                         GtObject* root = nullptr) override;

    /**
     * @brief startCommand
     * @param root
     * @param commandId
     * @return
     */
    GtCommand startCommand(GtObject* root,
                           const QString& commandId) override;

    /**
     * @brief endCommand
     */
    void endCommand(const GtCommand& command) override;

    /**
     * @brief Returns true if a command was started.
     * @return Whether a command was started or not
     */
    bool commandIsRunning();

    /**
     * @brief loadingProcedure
     * @param helper
     */
    void loadingProcedure(GtAbstractLoadingHelper* helper) override;

    /**
     * @brief Returns currently selected object.
     * @return Currently selected object.
     */
    GtObject* selectedObject();

    /**
     * @brief inDarkMode
     * @return true if application is in dark mode
     */
    Q_INVOKABLE bool inDarkMode();

    /**
     * @brief setDarkMode
     * @param dark - true if the app should be set to dark
     */
    void setDarkMode(bool dark);

    /** @brief getShortCutSequence
     * @param id - identification string of the short cut
     * @param category - if this is not empty the category of the registration
     * is use dto identify the short cut aswell
     * @return key sequence for the short cut
     */
    QKeySequence getShortCutSequence(const QString& id,
                                     const QString& category = "Core") const;

    /**
     * @brief compareKeyEvent
     * @param keyEvent - event to check
     * @param id - id of the short cut
     * @param category - category to use for the search (Core as default)
     * @return true if the event is ok for the short cut
     */
    bool compareKeyEvent(QKeyEvent* keyEvent,
                         const QString& id,
                         const QString& category = "Core") const;

    /**
     * @brief compareKeyEvent
     * @param keyEvent - event to check
     * @param k - key sequence to compare
     * @return true if the event is ok for the short cut
     */
    bool compareKeyEvent(QKeyEvent* keyEvent, const QKeySequence &k) const;

    /**
     * @brief shortCuts
     * @return pointer to the short cuts list object
     */
    GtShortCuts* shortCuts() const;

    /**
     * @brief extendShortCuts
     * @param list of short cuts from another modules
     */
    void extendShortCuts(QList<GtShortCutSettingsData>const& list);

    /**
     * @brief extendShortCuts
     * @param shortcut to add
     */
    void extendShortCuts(const GtShortCutSettingsData &shortcut);

    /**
     * @brief moduleShortCuts
     * @return shortcuts registered via interface
     */
    QList<GtShortCutSettingsData> moduleShortCuts() const;

    /**
     * @brief Requests the opening of the preferences dialog
     * on the page with the given title.
     * @param title Title of the page to be displayed first.
     */
    void showPreferences(const QString& title = "");


    using PageFactory = std::function<class GtPreferencesPage*()>;

    /**
     * @brief Adds a preference page to the preferences dialog
     *
     * Note, this method accepts a factory to be able to create
     * a page object on demand
     */
    static void addCustomPreferencePage(PageFactory);

    /**
     * @brief Returns the factories for all custom preference
     * pages that have been added by modules
     */
    static const QList<PageFactory>& customPreferencePages();

private:
    /// List of user specific perspective ids
    QStringList m_perspectiveIds;

    /// Current perspective
    std::unique_ptr<GtPerspective> m_perspective;

    /// gui specific module loader
    GtGuiModuleLoader* m_guiModuleLoader;

    /// Undo stack
    QUndoStack m_undoStack;

    /// Mutex for undo redo commands
    QMutex m_commandMutex;

    /// Pimpl
    GtApplicationPrivate* m_d;

    /// Stores currently selected object
    QPointer<GtObject> m_selectedObject;

    QList<GtShortCutSettingsData> m_moduleShortCuts;

    ///
    bool m_darkMode;

    /**
     * @brief readPerspectiveIds
     * @return
     */
    bool readPerspectiveIds();

    /**
     * @brief initFirstRun
     */
    bool initFirstRun() override;

private slots:
    /**
     * @brief onPropertyChange
     */
    void onPropertyChange(GtObject* obj, GtAbstractProperty* prop);

    /**
     * @brief Called after object was selected.
     * @param obj Currently selected object.
     */
    void onObjectSelected(GtObject* obj);

    /**
     * @brief onStackChange
     */
    void onUndoStackChange();

    /**
     * @brief Called to init modules after gui initialization.
     */
    void onGuiInitializationFinished();

signals:
    /**
     * @brief perspectiveChanged
     */
    void perspectiveChanged(QString);

    /**
     * @brief perspectiveAboutToBeChanged
     */
    void perspectiveAboutToBeChanged();

    /**
     * @brief perspectiveListChanged
     */
    void perspectiveListChanged();

    /**
     * @brief objectSelected
     * @param obj
     */
    void objectSelected(GtObject* obj);

    /**
     * @brief loadingProcedureRun
     * @param helper
     */
    void loadingProcedureRun(GtAbstractLoadingHelper* helper);

    /**
     * @brief themeChanged
     * @param dark - flag if new theme is dark or not
     */
    void themeChanged(bool dark);

    /**
     * @brief shortCutsChanged - signal to emit if the short cuts
     * had been modified
     */
    void shortCutsChanged();

    /**
     * @brief selectionByUuidRequested
     * Emit signal to select an object
     *  in the explorer dockwidget based on its uuid
     * @param uuid - uuid to the object to select in the dock widget
     */
    void selectionByUuidRequested(QString uuid);

    /**
     * @brief preferencesDialogRequested
     * Emit signal to open the preferences dialog
     *  on the page with the given title
     * @param title - Title of the preferences page to be displayed.
     */
    void preferencesDialogRequested(QString title);
};

#endif // GTAPPLICATION_H
