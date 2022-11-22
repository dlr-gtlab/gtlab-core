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
    GtTaskGroup(const QString& id);

    /**
     * @brief Destructor.
     */
    ~GtTaskGroup();

    // Initializes task group based on project path and scope
    bool init(const QString& projectPath,
              const GtTaskGroup::SCOPE scope);

    // Saves task group based on project path and scope
    bool save(const QString& projectPath,
              const GtTaskGroup::SCOPE scope) const;

    // Returns true if task group already initialized.
    // otherwise false will be returned.
    bool isInitialized();

    // Returns scope identification string based on given enum value
    static QString scopeId(const GtTaskGroup::SCOPE scope);

    static QString defaultUserGroupId();

    static QString groupPath(const QString& projectPath,
                             const GtTaskGroup::SCOPE scope,
                             const QString& groupId);

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
