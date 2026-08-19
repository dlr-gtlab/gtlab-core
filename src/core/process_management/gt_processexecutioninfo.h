/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_processexecutioninfo.h
 *
 *  Created on: 10.04.2026
 *  Author: Matthias Schuff (SG-VTM)
 *  Tel.:
 */
#ifndef GT_PROCESSEXECUTIONINFO_H
#define GT_PROCESSEXECUTIONINFO_H

#include "gt_object.h"
#include "gt_core_exports.h"
#include "gt_processcomponent.h"

class GT_CORE_EXPORT GtProcessExecutionInfo : public GtObject
{
    Q_OBJECT
public:
    Q_INVOKABLE GtProcessExecutionInfo();
    ~GtProcessExecutionInfo();

    double queuedTime() const;
    void setQueuedTime(double queuedTime);
    void setQueuedTimeNow();

    double startTime() const;
    void setStartTime(double startTime);
    void setStartTimeNow();

    double endTime() const;
    void setEndTime(double endTime);
    void setEndTimeNow();

    const GtProcessComponent::STATE processState() const;
    void setProcessState(GtProcessComponent::STATE state);

    GtObjectMementoDiff dataDiffToMerge() const;
    void setDataDiffToMerge(const GtObjectMementoDiff& dataDiffToMerge);

    void listEnumOptions();
private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

};

#endif // GT_PROCESSEXECUTIONINFO_H
