/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
     * @brief Returns a list of all root tasks of the current task group
     * @return Task list
     */
    QList<GtTask const*> currentProcessList() const;

    /**
     * @brief Returns a list of all root tasks of the task group sepcified
     * @param groupId
     * @return Task list
     */
    QList<GtTask const*> processList(const QString& groupId) const;

    /**
     * @brief Attempts to find the process by name in the current task group
     * @param name Process name
     * @return process
     */
    GtTask* findProcess(const QString& name);
    /**
     * @brief Attempts to find the process by name in the current task group
     * as const
     * @param name Process name
     * @return process
     */
    GtTask const* findProcess(const QString& name) const;

    /**
     * @brief Returns current task group
     * @return
     */
    GtTaskGroup* taskGroup();
    /**
     * @brief Returns current task group as const
     * @return
     */
    GtTaskGroup const* taskGroup() const;

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
     * @brief Creates new task group.
     * @param taskGroupId Identification string of new task group
     * @param scope Task group target scope
     * @return newly created task group. nullptr is returned if task group
     * creation failed.
     */
    GtTaskGroup* createNewTaskGroup(const QString& taskGroupId,
                                    GtTaskGroup::SCOPE scope);
    /**
     * @brief Renames task group
     * @param taskGroupId Identification string of task group to be renamed
     * @param taskGroupIdNew New task group identification string
     * @param scope
     * @param projectPath
     * @return true if task group could be successfully renamed. otherwise
     * false is returned
     */
    bool renameTaskGroup(const QString& taskGroupId,
                         const QString& taskGroupIdNew,
                         GtTaskGroup::SCOPE scope,
                         const QString& projectPath);

    /**
     * @brief Deletes the specified Task Group from the process data.
     * @param taskGroupId The ID of the Task Group to be deleted.
     * @param scope The scope in which the Task Group is located.
     * @return True if the Task Group was successfully deleted; otherwise false.
     */
    bool deleteTaskGroup(const QString& taskGroupId,
                         GtTaskGroup::SCOPE scope);

    /**
     * @brief Initializes all Task Groups for the given project.
     * @param projectPath The path to the project directory.
     * @return True if all Task Groups were successfully initialized;
     * otherwise false.
     */
    bool initAllTaskGroups(const QString& projectPath);

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
