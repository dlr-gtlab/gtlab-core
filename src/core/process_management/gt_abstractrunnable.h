/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 27.01.2016
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#ifndef GTABSTRACTRUNNABLE_H
#define GTABSTRACTRUNNABLE_H

#include "gt_core_exports.h"

#include <QRunnable>

#include "gt_object.h"
#include "gt_objectpath.h"
#include "gt_objectmemento.h"

class QDir;
class GtProcessComponent;

/**
 * @brief The GtAbstractRunnable class
 */
class GT_CORE_EXPORT GtAbstractRunnable : public QObject,
    public QRunnable
{
    Q_OBJECT

public:

    /**
     * @brief Constructor.
     */
    GtAbstractRunnable();

    /**
     * @brief Destructor.
     */
    virtual ~GtAbstractRunnable();

    /**
     * @brief linkedObjects
     * @return
     */
    QList<GtObject*> linkedObjects();

    /**
     * @brief appendSourceData
     * @param memento
     */
    void appendSourceData(const GtObjectMemento& memento);

    /**
     * @brief sourceData
     * @return
     */
    QList<GtObjectMemento>& outputData();

    /**
     * @brief Appends additional process component to runnable queue.
     * NOTE: Runnable takes ownership of process component.
     * @param Process component pointer.
     * @return Returns true if process component was successfully appended,
     * otherwise return false;
     */
    bool appendProcessComponent(GtProcessComponent* comp);

    /**
     * @brief Returns current queue.
     * @return List of process components waiting in runnable queue.
     */
    QList<GtProcessComponent*> queue();

    /**
     * @brief Returns process specific temporary directory.
     * @return
     */
    virtual QDir tempDir() = 0;

    /**
     * @brief clearTempDir
     * @param path
     * @return
     */
    virtual bool clearTempDir(const QString& path) = 0;

    /**
     * @brief Returns path of current project.
     * @return Path to current project.
     */
    virtual QString projectPath() = 0;

    /**
     * @brief successful
     * @return
     */
    bool successful();

    /**
     * @brief Returns datamodel object based on given object uuid. If no
     * object is found nullpointer is returned.
     * @tparam T Object type
     * @param uuid Object uuid string
     * @return Object corresponding to given uuid
     */
    template <class T>
    T data(QString uuid) const
    {
        if (uuid.isEmpty())
        {
            return Q_NULLPTR;
        }

        foreach (GtObject* obj, m_linkedObjects)
        {
            if (obj == Q_NULLPTR)
            {
                continue;
            }

            GtObject* retval = obj->getObjectByUuid(uuid);

            if (retval != Q_NULLPTR)
            {
                return qobject_cast<T>(retval);
            }
        }

        return Q_NULLPTR;
    }

    /**
     * @brief Returns datamodel object based on given object path. If no
     * object is found nullpointer is returned.
     * @tparam T Object type
     * @param path Object path
     * @return Object corresponding to given object path
     */
    template <class T>
    T data(const GtObjectPath& path) const
    {
        if (path.isEmpty())
        {
            return Q_NULLPTR;
        }

        foreach (GtObject* obj, m_linkedObjects)
        {
            GtObject* retval = path.getObject(obj);

            if (retval != Q_NULLPTR)
            {
                return qobject_cast<T>(retval);
            }
        }

        return Q_NULLPTR;
    }

protected:
    ///
    QList<GtObject*> m_linkedObjects;

    /// List of output meta data
    QList<GtObjectMemento> m_outputData;

    /// List of input meta data
    QList<GtObjectMemento> m_inputData;

    /// Process component queue
    QList<GtProcessComponent*> m_queue;

    ///
    bool m_successfulRun;

signals:
    /**
     * @brief runnableFinished
     */
    void runnableFinished();

};

#endif // GTABSTRACTRUNNABLE_H
