/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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

    const double queuedTime();
    void setQueuedTime(double queuedTime);
    void setQueuedTimeNow();

    const double startTime();
    void setStartTime(double startTime);
    void setStartTimeNow();

    const double endTime();    
    void setEndTime(double endTime);
    void setEndTimeNow();

    const GtProcessComponent::STATE processState();
    void setProcessState(GtProcessComponent::STATE state);

    const GtObjectMementoDiff dataDiffToMerge();
    void setDataDiffToMerge(const GtObjectMementoDiff& dataDiffToMerge);

    void listEnumOptions();
private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

};

#endif // GT_PROCESSEXECUTIONINFO_H
