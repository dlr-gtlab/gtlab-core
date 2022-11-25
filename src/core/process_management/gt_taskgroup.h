/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 12.10.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKGROUP_H
#define GTTASKGROUP_H

#include "gt_core_exports.h"

#include <QString>

#include "gt_objectgroup.h"

#include <memory>

class QDir;
class QDomElement;
class GtTask;

class GT_CORE_EXPORT GtTaskGroup : public GtObjectGroup
{
    Q_OBJECT

public:
    enum SCOPE
    {
        USER,
        CUSTOM,
        UNDEFINED
    };

    /**
     * @brief Constructor
     * @param id Identification string of task group
     */
    explicit GtTaskGroup(const QString& id);

    ~GtTaskGroup() override;

    /**
     * @brief Initializes task group based on project path and scope.
     * @param projectPath
     * @param scope
     * @return
     */
    bool read(const QString& projectPath,
              const GtTaskGroup::SCOPE scope);

    /**
     * @brief Saves task group based on project path and scope.
     * @param projectPath
     * @param scope
     * @return
     */
    bool save(const QString& projectPath,
              const GtTaskGroup::SCOPE scope) const;

    /**
     * @brief Returns true if task group already initialized.
     * otherwise false will be returned.
     * @return
     */
    bool isInitialized() const;

    /**
     * @brief Returns scope identification string based on given enum value.
     * @param scope
     * @return
     */
    static QString scopeId(const GtTaskGroup::SCOPE scope);

    /**
     * @brief Returns default identification strig of user group based on
     * system user.
     * @return
     */
    static QString defaultUserGroupId();

    /**
     * @brief Returns path to task group path on filesystem.
     * @param projectPath
     * @param scope
     * @param groupId
     * @return
     */
    static QString groupPath(const QString& projectPath,
                             const GtTaskGroup::SCOPE scope,
                             const QString& groupId);

    /**
     * @brief Stores given task xml element to corresponding path on filesystem.
     * @param projectPath
     * @param scope
     * @param groupId
     * @param taskElement
     * @return
     */
    static bool saveTaskElementToFile(const QString&  projectPath,
                                      const GtTaskGroup::SCOPE scope,
                                      const QString& groupId,
                                      const QDomElement& taskElement);

private:
    /// Private implementation
    class Impl;
    std::unique_ptr<Impl> m_pimpl;

};

#endif // GTTASKGROUP_H
