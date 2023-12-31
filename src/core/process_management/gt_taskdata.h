/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_taskdataimpl.h
 *
 *  Created on: 03.11.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_TASKDATA_H
#define GT_TASKDATA_H

#include "gt_core_exports.h"

#include "gt_abstractprocessdata.h"
#include "gt_versionnumber.h"

#include <QStringList>
#include <QMetaObject>

class GtTaskDataImpl;

#if defined(_MSC_VER) || defined(__APPLE__)
#include <memory>
typedef std::shared_ptr<GtTaskDataImpl> GtTaskData;
#else
#include <tr1/memory>
typedef std::tr1::shared_ptr<GtTaskDataImpl> GtTaskData;
#endif

#define GT_TASK_DATA(A) GtTaskDataImpl::newTaskData(A::staticMetaObject)

/**
 * @brief The GtTaskDataImpl class
 */
class GT_CORE_EXPORT GtTaskDataImpl : public GtAbstractProcessData
{
public:
    /**
     * @brief GtTaskDataImpl
     * @param metaData
     */
    explicit GtTaskDataImpl(const QMetaObject& metaData);

    /**
     * @brief Returns calculator meta data.
     * @return Calculator meta data.
     */
    const QMetaObject& metaData() const;

    /**
     * @brief Returns whether calculator data is valid or not.
     * @return Validation.
     */
    bool isValid() const override;

    /**
     * @brief newTaskData
     * @param metaData
     * @return
     */
    static GtTaskData newTaskData(const QMetaObject& metaData);

    /// Task identification string.
    QString id;

    /// Task description.
    QString description;

    /// Task author.
    QString author;

    /// Task author contact.
    QString contact;

    /// Task author company.
    QString company;

    /// Task category.
    QString category;

    /// Task version.
    GtVersionNumber version;

private:
    /// Task meta data.
    QMetaObject m_metaData;
};

#endif // GT_TASKDATA_H
