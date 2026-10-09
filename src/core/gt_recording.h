/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_command.h
 *
 *  Created on: 28.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#pragma once

#include "gt_object.h"

#include <qobject.h>
#include <QPointer>
#include <QSet>
#include <QString>
#include <QUuid>

#include "gt_core_exports.h"


/**
 * @brief The GtRecording class
 */
class GT_CORE_EXPORT GtRecording
{
public:

    /**
     * @brief Constructor.
     */
    GtRecording();

    /**
     * @brief Returns command identification string.
     * @return Command identification string
     */
    QUuid contextUuid() const;
    /**
     * @brief Returns command identification string.
     * @return Command identification string
     */
    QString startAtTime() const;
    /**
     * @brief Returns command identification string.
     * @return Command identification string
     */
    QString endedAtTime() const;
    /**
     * @brief Returns command identification string.
     * @return Command identification string
     */
    QSet<QUuid> childContextUuids() const;


    QPointer<GtObject> activityObject() const;

    QString State() const;
    void setState(const QString &newState);

    QString Version() const;
    void setVersion(const QString &newVersion);

    QString toolName() const;
    void setToolName(const QString &newToolName);

    QList<QPointer<GtObject> > linkedObjects() const;

    /**
     * @brief Sets the activity object.
     * @param activityObject
     */
    void
    setActivityObject(QPointer<GtObject> activityObject);

    /**
     * @brief Sets the linked objects.
     * @param linkedObjects
     */
    void
    setLinkedObjects(QList<QPointer<GtObject>> linkedObjects);

    /**
     * @brief Sets the start time.
     * @param startAtTime
     */
    void
    setStartAtTime(const QString& startAtTime);

    /**
     * @brief Sets the end time.
     * @param endedAtTime
     */
    void
    setEndedAtTime(const QString& endedAtTime);

private:
    /// Command identification string
    QUuid m_actvityUuid;

    QSet<QUuid> m_childContextUuids;

    QString m_startAtTime;

    QString m_endedAtTime;

    QString m_State;

    QString m_Version;

    QString m_toolName;

    QPointer<GtObject> m_activityObject;

    QList<QPointer<GtObject>> m_linkedObjects;
};

class GT_CORE_EXPORT GtAbstractRecorder
{
public:
    explicit
        GtAbstractRecorder() {};

    virtual bool
    initLinkedObjects(const QList<QPointer<GtObject>> linkedObjects)=0;

    virtual bool
    recordChanges(const QList<QPointer<GtObject>> linkedObjects)=0;

    virtual bool
    recordAccessObjects(const QSet<QUuid> accessedObjects, QList<QPointer<GtObject>> linkedObjects)=0;

    virtual bool
    createActivity(const GtRecording& recording)=0;

};

class GtAbstractRunnable;

namespace gt
{
/**
 * @brief Starts the access recording for the given execution context.
 * @param recorder
 * @param activityObject
 * @param linkedObjects
 * @param runnable Execution context to record the property accesses of
 * @return New recording
 */
GT_CORE_EXPORT GtRecording
startAccessRecording(GtAbstractRecorder* recorder,
                     QPointer<GtObject> activityObject,
                     QList<QPointer<GtObject> > linkedObjects,
                     const GtAbstractRunnable& runnable);
/**
 * @brief Ends the access recording for the given execution context.
 * @param recorder
 * @param recording
 * @param runnable Execution context whose property accesses are recorded
 */
GT_CORE_EXPORT void
endAccessRecording(GtAbstractRecorder* recorder,
                   GtRecording& recording,
                   const GtAbstractRunnable& runnable);
}
