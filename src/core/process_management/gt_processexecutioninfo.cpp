/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processexecutioninfo.cpp
 *
 *  Created on: 10.04.2026
 *  Author: Matthias Schuff (SG-VTM)
 *  Tel.:
 */
#include "gt_processexecutioninfo.h"
#include "gt_doubleproperty.h"
#include "gt_objectmementodiff.h"
#include "gt_processcomponent.h"
#include "gt_enumproperty.h"

#include <QDateTime>
#include <QDomDocument>


struct GtProcessExecutionInfo::Impl
{
    Impl()
    {
        queuedTime.setReadOnly(true);
        queuedTime.setOptional(true);
        queuedTime.setActive(false);

        startTime.setReadOnly(true);
        startTime.setOptional(true);
        startTime.setActive(false);

        endTime.setReadOnly(true);
        endTime.setOptional(true);
        endTime.setActive(false);

        processState.setReadOnly(true);
        dataDiffToMerge.setReadOnly(true);
    }

    GtDoubleProperty queuedTime{"queuedTime", "Queued Time", "", GtUnit::Time, -1.0};
    GtDoubleProperty startTime{"startTime", "Start Time", "", GtUnit::Time, -1.0};
    GtDoubleProperty endTime{"endTime", "End Time", "", GtUnit::Time, -1.0};
    GtEnumProperty<GtProcessComponent::STATE> processState{"processState", "Process State", ""};
    GtStringProperty dataDiffToMerge{"dataDiffToMerge", "Data To Merge", ""};
};

GtProcessExecutionInfo::GtProcessExecutionInfo():
    pimpl(std::make_unique<Impl>())
{
    registerProperty(pimpl->queuedTime);
    registerProperty(pimpl->startTime);
    registerProperty(pimpl->endTime);
    registerProperty(pimpl->processState);
    registerProperty(pimpl->dataDiffToMerge);
}

const double GtProcessExecutionInfo::queuedTime()
{
    return pimpl->queuedTime.getVal();
}

void GtProcessExecutionInfo::setQueuedTime(double queuedTime)
{
    pimpl->queuedTime = queuedTime;
    pimpl->queuedTime.setActive(true);
}

void GtProcessExecutionInfo::setQueuedTimeNow()
{
    setQueuedTime(QDateTime::currentMSecsSinceEpoch()/1000.);
}

const double GtProcessExecutionInfo::startTime()
{
    return pimpl->startTime.getVal();
}

void GtProcessExecutionInfo::setStartTime(double startTime)
{
    pimpl->startTime = startTime;
    pimpl->startTime.setActive(true);
}

void GtProcessExecutionInfo::setStartTimeNow()
{
    setStartTime(QDateTime::currentMSecsSinceEpoch()/1000.);
}

const double GtProcessExecutionInfo::endTime()
{
    return pimpl->endTime.getVal();
}

void GtProcessExecutionInfo::setEndTime(double endTime)
{
    pimpl->endTime.setVal(endTime);
    pimpl->endTime.setActive(true);
}

void GtProcessExecutionInfo::setEndTimeNow()
{
    setEndTime(QDateTime::currentMSecsSinceEpoch()/1000.);
}

const GtProcessComponent::STATE GtProcessExecutionInfo::processState()
{
    return pimpl->processState.getVal();
}

void GtProcessExecutionInfo::setProcessState(GtProcessComponent::STATE state)
{
    pimpl->processState.setVal(state);
}

const GtObjectMementoDiff GtProcessExecutionInfo::dataDiffToMerge()
{
    GtObjectMementoDiff diff;

    // QDomDocument reader setContent in GtObjectMementoDiff
    // constructor accepts only one <object> on hightest level
    QString str = "<mementodifflist>\n"+pimpl->dataDiffToMerge.getVal()+"\n</mementodifflist>";

    QDomDocument doc;
    doc.setContent(str);

    QDomElement root = doc.documentElement();
    QDomNodeList diffs = root.childNodes();
    for (int i = 0; i < diffs.size(); ++i) {
        QDomNode node = diffs.at(i);

        QDomDocument tempDoc;
        QDomNode imported = tempDoc.importNode(node, true);
        tempDoc.appendChild(imported);

        GtObjectMementoDiff tempDiff{tempDoc.toByteArray()};
        diff.appendDiff(tempDiff);
    }
    return diff;
}

void GtProcessExecutionInfo::setDataDiffToMerge(const GtObjectMementoDiff &dataDiffToMerge)
{
    pimpl->dataDiffToMerge.setVal(QString::fromUtf8(dataDiffToMerge.toByteArray()));
}

GtProcessExecutionInfo::~GtProcessExecutionInfo() = default;

void GtProcessExecutionInfo::listEnumOptions()
{
    QMetaEnum metaEnum = pimpl->processState.getMetaEnum();
    for (int i = 0; i < metaEnum.keyCount(); ++i) {
        const char* key = metaEnum.key(i);
        int value = metaEnum.value(i);
        qDebug().noquote() << key << "=" << value;
    }
}
