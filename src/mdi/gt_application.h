/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 07.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTAPPLICATION_H
#define GTAPPLICATION_H

#include "gt_mdi_exports.h"

#include "gt_coreapplication.h"

#if defined(gtApp)
#undef gtApp
#endif
#define gtApp (static_cast<GtApplication *>(GtCoreApplication::instance()))

#include <QUndoStack>
#include <QMutex>
#include <QPointer>

class GtPerspective;
class GtObjectUI;
class GtObject;
class GtGuiModuleLoader;
class GtAbstractProperty;
class GtProcessExecutor;
class GtApplicationPrivate;

class GT_MDI_EXPORT GtApplication : public GtCoreApplication
{
    Q_OBJECT

public:
    /**
     * @brief GtApplication
     */
    GtApplication(QCoreApplication* parent);

    /**
     * @brief ~GtApplication
     */
    virtual ~GtApplication();

    /** Creates a new icon from resource file.
        @param iconName Name of icon file */
    static QIcon icon(const QString iconName);

    /**
     * @brief initMdiLauncher
     */
    void initMdiLauncher();

    /**
     * @brief loadModules
     */
    virtual void loadModules();

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
     * @brief objectUI
     * @param obj
     * @return
     */
    GtObjectUI* objectUI(GtObject* obj);

    /**
     * @brief objectUI
     * @param classname
     * @return
     */
    GtObjectUI* objectUI(const QString& classname);

    /**
     * @brief knownUIObjects - returns a list of all stored object ui classes
     * @return class name list
     */
    QStringList knownUIObjects();

    /**
     * @brief switchSession
     * @param id
     */
    virtual void switchSession(const QString& id);

    /**
     * @brief undoStack
     * @return
     */
    QUndoStack* undoStack();

    /**
     * @brief propertyCommand
     * @param obj
     * @param prop
     * @param newValue
     * @param unit
     * @param root
     */
    void propertyCommand(GtObject* obj, GtAbstractProperty* prop,
                         const QVariant& newValue,
                         const QString& unit = QString(),
                         GtObject* root = Q_NULLPTR);

    /**
     * @brief startCommand
     * @param root
     * @param commandId
     * @return
     */
    GtCommand startCommand(GtObject* root,
                           const QString& commandId) Q_DECL_OVERRIDE;

    /**
     * @brief endCommand
     */
    void endCommand(const GtCommand& command) Q_DECL_OVERRIDE;

    /**
     * @brief Returns true if a command was started.
     * @return Whether a command was started or not
     */
    bool commandIsRunning();

    /**
     * @brief loadingProcedure
     * @param helper
     */
    void loadingProcedure(GtAbstractLoadingHelper* helper) Q_DECL_OVERRIDE;

    /**
     * @brief Returns currently selected object.
     * @return Currently selected object.
     */
    GtObject* selectedObject();

private:
    /// List of user specific perspective ids
    QStringList m_perspectiveIds;

    /// Current perspective
    GtPerspective* m_perspective;

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

    /**
     * @brief readPerspectiveIds
     * @return
     */
    bool readPerspectiveIds();

    /**
     * @brief initFirstRun
     */
    bool initFirstRun();

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

};

#endif // GTAPPLICATION_H