/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMDIITEM_H
#define GTMDIITEM_H

#include "gt_gui_exports.h"

#include <QPointer>

#include "gt_object.h"
//#include "gt_variantproperty.h"

class QFrame;
class QSettings;
class QMdiSubWindow;
class GtProject;
class GtQueuedMdiEvent;
class GtAbstractProperty;
class GtMdiLauncher;
class GtState;

/**
 * @brief The GtMdiItem class
 */
class GT_GUI_EXPORT GtMdiItem : public GtObject
{
    Q_OBJECT

    friend class GtMdiLauncher;

public:
    /**
     * @brief widget
     * @return
     */
    QWidget* widget();

    /**
     * @brief Virtual function to specify item specific icon.
     * @return Object specific icon.
     */
    virtual QIcon icon() const;

    /**
     * @brief setData
     * @param obj
     */
    virtual void setData(GtObject* obj);

    /** Event called after current project has changed.
        @param new current project */
    virtual void projectChangedEvent(GtProject* project);

    /**
     * @brief Event called after object selection has changed.
     * @param current object
     */
    virtual void objectChangedEvent(GtObject* obj);

    /**
     * @brief showEvent
     */
    virtual void showEvent();

    /**
     * @brief Event called after project was saved.
     * @param project Savend project.
     */
    virtual void projectSavedEvent(GtProject* project);

    /**
     * @brief Initializes a new global state.
     * @param State identification string.
     * @param State specific path.
     * @param Initial value.
     * @param Force initialization indicator.
     * @return New state.
     */
    GtState* initializeState(const QString& id, const QString& path,
                             const QVariant& initValue,
                             bool forceInitialization = false);

    /**
     * @brief Initializes a new project specific state.
     * @param Project.
     * @brief Initializes a new global state.
     * @param State identification string.
     * @param State specific path.
     * @param Initial value.
     * @param Force initialization indicator.
     * @return New state.
     */
    GtState* initializeState(GtProject* project,const QString& id,
                             const QString& path, const QVariant& initValue,
                             bool forceInitialization = false);

    /**
      * @brief subWin
      * @return
      */
    QMdiSubWindow* subWin();

    /**
     * @brief Virtual function to specify if mdi item allows multiple instances.
     * @return Multiple instances indicator.
     */
    virtual bool allowsMultipleInstances() const;

public slots:
    /**
     * @brief print
     */
    virtual void print();

    /**
     * @brief onThemeChanged
     * Implementation to hande the change betewwn dark and bright mode
     */
    virtual void onThemeChanged();

protected:
    /**
     * @brief GtMdiItem
     */
    GtMdiItem();

    /**
     * @brief ~GtMdiItem
     */
    ~GtMdiItem() override;

    /**
     * @brief initialized
     */
    virtual void initialized();

    /**
     * @brief Virtual function to return custom mdi id based on given object.
     * Default implementation returns empty string.
     * @param obj Given object
     * @return Custom mdi identification string.
     */
    virtual QString customId(GtObject* obj);

    /**
     * @brief setSubWin
     * @param subWin
     */
    void setSubWin(QMdiSubWindow* subWin);

private:
    ///
    QPointer<QFrame> m_frame;

    /// Mdi item data
    QPointer<GtObject> m_d;

    ///
    QList<GtQueuedMdiEvent*> m_eventQueue;

    ///
    QMdiSubWindow* m_subWin;

    /// Mdi item custom identification string
    QString m_cid;

    ///
    bool m_queueEvents;

    template <class T>
    T* takeEvent()
    {
        foreach (GtQueuedMdiEvent* e, m_eventQueue)
        {
            if (T* ce = qobject_cast<T*>(e))
            {
                m_eventQueue.removeOne(e);
                return ce;
            }
        }

        return NULL;
    }

private slots:
    /** Called after current project has changed.
        @param project new current project */
    void onProjectChanged(GtProject* project);

    /**
     * @brief Called after project was saved.
     * @param project Saved project.
     */
    void onProjectSaved(GtProject* project);

    /** Called after current project has changed.
        @param project new current project */
    void onObjectChanged(GtObject* obj);

    /**
     * @brief windowStateChanged
     * @param oldState
     * @param newState
     */
    void windowStateChanged(Qt::WindowStates oldState,
                            Qt::WindowStates newState);

    /**
     * @brief windowAboutToActive
     */
    void windowAboutToActive();

signals:
    /**
     * @brief try to reopening mdi item (Result View)
     * @param pointer to existing data object
     */
    void tryReopening(GtObject* existDataObj);

};

#endif // GTMDIITEM_H
