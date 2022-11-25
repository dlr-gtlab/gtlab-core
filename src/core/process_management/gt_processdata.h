/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 05.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSDATA_H
#define GTPROCESSDATA_H

#include "gt_core_exports.h"
#include "gt_objectgroup.h"
#include "gt_taskgroup.h"

#include <memory>

class GtTask;

/**
 * @brief The GtProcessData class
 */
class GT_CORE_EXPORT GtProcessData : public GtObjectGroup
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessData
     */
    Q_INVOKABLE GtProcessData();

    ~GtProcessData() override;

    /**
     * @brief processList
     * @return
     */
    QList<GtTask*> processList();

    /**
     * @brief findProcess
     * @param val
     * @return
     */
    GtTask* findProcess(const QString& val);

    /**
     * @brief Returns current task group
     * @return
     */
    GtTaskGroup* taskGroup();

    /**
     * @brief Initializes process data for given project
     * @param projectPath
     * @return false if process data is already initialized
     */
    bool read(const QString& projectPath);

    /**
     * @brief Switches current task group
     * @param taskGroupId
     * @param scope
     * @param projectPath
     * @return
     */
    bool switchCurrentTaskGroup(const QString& taskGroupId,
                                GtTaskGroup::SCOPE scope,
                                const QString& projectPath);

    /**
     * @brief Stores process data for given project
     * @param projectPath
     * @return false if process data is already initialized or saving is failed.
     */
    bool save(const QString& projectPath) const;

    /**
     * @brief Returns list of task group identification strings in user scope.
     * @return
     */
    QStringList userGroupIds() const;

    /**
     * @brief Returns list of task group identification strings in custom scope.
     * @return
     */
    QStringList customGroupIds() const;

private:
    /// Private implementation
    class Impl;
    std::unique_ptr<Impl> m_pimpl;

};

#endif // GTPROCESSDATA_H
