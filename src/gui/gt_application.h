﻿/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
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
                           bool batchMode = false);

    /**
     * @brief Destructor.
     */
    ~GtApplication() override;

    /** Creates a new icon from a resource file.
     *  @param iconPath Relative or custom resource path to a icon file.
     */
    static QIcon icon(QString iconPath);

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
    bool inDarkMode();

    /**
     * @brief setDarkMode
     * @param dark - true if the app should be set to dark
     * @param initial -  true if this is the first set
     */
    void setDarkMode(bool dark, bool initial = false);

    /** @brief getShortCutSequence
     * @param id - identification string of the short cut
     * @return key sequence for the short cut
     */
    QKeySequence getShortCutSequence(const QString& id) const;

    /**
     * @brief compareKeyEvent
     * @param event - event to check
     * @param id - id of the short cut
     * @return true if the event is ok for the short cut
     */
    bool compareKeyEvent(QKeyEvent* event, const QString& id) const;

    /**
     * @brief shortCuts
     * @return pointer to the short cuts list object
     */
    GtShortCuts* shortCuts() const;

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

    /// Process executor in gui mode
    GtProcessExecutor* m_processExecutor;

    /// Pimpl
    GtApplicationPrivate* m_d;

    /// Stores currently selected object
    QPointer<GtObject> m_selectedObject;

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
};

#endif // GTAPPLICATION_H