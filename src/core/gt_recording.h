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

#include "gt_core_exports.h"


/**
 * @brief The GtRecording class
 */
class GT_CORE_EXPORT GtRecording
{
    friend class GtCoreApplication;

public:

    /**
     * @brief Returns command identification string.
     * @return Command identification string
     */
    QString contextUuid() const;
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
    QSet<QString> childContextUuids() const;


    QPointer<GtObject> activityObject() const;

    QString State() const;
    void setState(const QString &newState);

    QString Version() const;
    void setVersion(const QString &newVersion);

    QString toolName() const;
    void setToolName(const QString &newToolName);

private:
    /**
     * @brief Constructor.
     * @param uuid Command identification string
     */
    explicit GtRecording();

    /// Command identification string
    QString m_actvityUuid;

    QSet<QString> m_childContextUuids;

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
    recordAccessObjects(const QSet<QString> accessedObjects, QList<QPointer<GtObject>> linkedObjects)=0;

    virtual bool
    createActivity(const GtRecording& recording)=0;

};
