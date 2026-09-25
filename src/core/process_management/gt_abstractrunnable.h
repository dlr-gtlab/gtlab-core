/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
    ~GtAbstractRunnable() override;

    /**
     * @brief linkedObjects
     * @return
     */
    const QList<GtObject*>& linkedObjects() const;

    /**
     * @brief setLinkedObjects
     * @param objects
     */
    void setLinkedObjects(const QList<GtObject*>& objects);

    /**
     * @brief addLinkedObject
     * @param object
     */
    void addLinkedObject(GtObject* object);

    /**
     * @brief appendSourceData
     * @param memento
     */
    void appendSourceData(const GtObjectMemento& memento);

    /**
     * @brief setInputData
     * @param data
     */
    void setInputData(const QList<GtObjectMemento>& data);

    /**
     * @brief sourceData
     * @return
     */
    const QList<GtObjectMemento>& outputData() const;

    /**
     * @brief inputData
     * @return
     */
    const QList<GtObjectMemento>& inputData() const;

    /**
     * @brief Appends additional process component to runnable queue.
     * NOTE: Runnable takes ownership of process component.
     * @param comp Process component pointer.
     * @return Returns true if process component was successfully appended,
     * otherwise return false;
     */
    bool appendProcessComponent(GtProcessComponent* comp);

    /**
     * @brief Returns current queue.
     * @return List of process components waiting in runnable queue.
     */
    const QList<GtProcessComponent*>& queue() const;

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
     * @brief DOE execution context structure
     * Contains metadata for tracking parallel execution runs
     */
    struct DOEContext
    {
        int iteration{0};
        int runIndex{0};
        int totalRuns{0};
        QString runId;
    };

    /**
     * @brief setDOEContext
     * @param context
     */
    void setDOEContext(const DOEContext& context);

    /**
     * @brief doeContext
     * @return
     */
    DOEContext doeContext() const;

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
            return nullptr;
        }

        for (GtObject* obj : qAsConst(m_linkedObjects))
        {
            if (!obj)
            {
                continue;
            }

            if (GtObject* retval = obj->getObjectByUuid(uuid))
            {
                return qobject_cast<T>(retval);
            }
        }

        return nullptr;
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
            return nullptr;
        }

        // An STL algorithm makes no really sense here since we
        // would need to call getObject twice
        for (GtObject* obj : qAsConst(m_linkedObjects))
        {
            if (GtObject* retval = path.getObject(obj))
            // cppcheck-suppress useStlAlgorithm
            {
                return qobject_cast<T>(retval);
            }
        }

        return nullptr;
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

    /// DOE execution context for parallel runs
    DOEContext m_doeContext;

signals:
    /**
     * @brief runnableFinished
     */
    void runnableFinished();
};

#endif // GTABSTRACTRUNNABLE_H
