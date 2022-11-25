/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 05.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QDirIterator>

#include "gt_task.h"
#include "gt_objectgroup.h"
#include "gt_taskgroup.h"

#include "gt_processdata.h"

class GtProcessData::Impl
{
public:
    std::reference_wrapper<GtProcessData> _pub;

    int _initialized = false;

    GtTaskGroup::SCOPE _currentScope = GtTaskGroup::USER;

    QString _currentGroupId;

    explicit Impl (GtProcessData& pub) : _pub(pub) { }

    bool readTaskGroups(const QString& projectPath, GtTaskGroup::SCOPE scope);

    bool initTaskGroup(const QString& groupId,
                       const QString& projectPath,
                       GtTaskGroup::SCOPE scope);

    bool initDefaultUserGroup(const QString& projectPath);

    GtObjectGroup* groupContainer(GtTaskGroup::SCOPE scope) const;

    GtTaskGroup* currentTaskGroup() const;

    QList<GtTaskGroup*> userGroups();

    QList<GtTaskGroup*> customGroups();

};

GtProcessData::GtProcessData() :
    m_pimpl{std::make_unique<Impl>(*this)}
{
    setObjectName(QStringLiteral("Process Data"));

    // add default container for user task groups
    GtObjectGroup* userGroups = new GtObjectGroup;
    userGroups->setObjectName(GtTaskGroup::scopeId(GtTaskGroup::USER));
    userGroups->setDefault(true);
    appendChild(userGroups);

    // add default container for custom task groups
    GtObjectGroup* customGroups = new GtObjectGroup;
    customGroups->setObjectName(GtTaskGroup::scopeId(GtTaskGroup::CUSTOM));
    customGroups->setDefault(true);
    appendChild(customGroups);

    setUserHidden(true);
}

GtProcessData::~GtProcessData() = default;

QList<GtTask*>
GtProcessData::processList()
{
    GtTaskGroup* currentGroup = m_pimpl->currentTaskGroup();

    if (!currentGroup)
    {
        gtError() << tr("Current task group not found!");
        return {};
    }

    return currentGroup->findDirectChildren<GtTask*>();
}

GtTask*
GtProcessData::findProcess(const QString& val)
{
    GtTaskGroup* currentGroup = m_pimpl->currentTaskGroup();

    if (!currentGroup)
    {
        gtError() << tr("Current task group not found!");
        return nullptr;
    }

    return currentGroup->findDirectChild<GtTask*>(val);
}

GtTaskGroup*
GtProcessData::taskGroup()
{
    return m_pimpl->currentTaskGroup();
}

const GtTaskGroup*
GtProcessData::taskGroup() const
{
    return m_pimpl->currentTaskGroup();
}

bool
GtProcessData::read(const QString& projectPath)
{
    if (m_pimpl-> _initialized)
    {
        gtError() << tr("Task group was already initialized!");
        return false;
    }

    // search all diectories
    if (!m_pimpl->readTaskGroups(projectPath, GtTaskGroup::USER))
    {
        return false;
    }

    if (!m_pimpl->readTaskGroups(projectPath, GtTaskGroup::CUSTOM))
    {
        return false;
    }


    // init last active task group
    if (!m_pimpl->initTaskGroup(GtTaskGroup::defaultUserGroupId(), projectPath,
                                GtTaskGroup::USER))
    {
        // last active task group not found. init default user group
        if (!m_pimpl->initDefaultUserGroup(projectPath))
        {
            return false;
        }
    }

    // initialization finished. Yippee-ki-yay, mot*********
    m_pimpl-> _initialized = true;

    return true;
}

bool
GtProcessData::switchCurrentTaskGroup(const QString& taskGroupId,
                                      GtTaskGroup::SCOPE scope,
                                      const QString& projectPath)
{
    if (!m_pimpl-> _initialized)
    {
        gtError() << tr("Process data is not initialized!") << tr("Call init() first!");
        return false;
    }

    return m_pimpl->initTaskGroup(taskGroupId, projectPath, scope);
}

bool
GtProcessData::save(const QString& projectPath) const
{
    if (!m_pimpl-> _initialized)
    {
        gtError() << tr("Cannot save an uninitialized task group!");
        return false;
    }

    foreach (auto* group, m_pimpl->userGroups())
    {
        if (!group->save(projectPath, GtTaskGroup::USER))
        {
            return false;
        }
    }

    foreach (auto* group, m_pimpl->customGroups())
    {
        if (!group->save(projectPath, GtTaskGroup::CUSTOM))
        {
            return false;
        }
    }

    return true;
}

QStringList
GtProcessData::userGroupIds() const
{
    QStringList retval;

    foreach (auto* group, m_pimpl->userGroups())
    {
        retval << group->objectName();
    }

    return retval;
}

QStringList
GtProcessData::customGroupIds() const
{
    QStringList retval;

    foreach (auto* group, m_pimpl->customGroups())
    {
        retval << group->objectName();
    }

    return retval;
}

bool
GtProcessData::Impl::readTaskGroups(const QString& projectPath,
                                    GtTaskGroup::SCOPE scope)
{
    QString scopeId = GtTaskGroup::scopeId(scope);

    if (scopeId.isEmpty())
    {
        gtError() << QObject::tr("Invalid task group scope");
        return false;
    }

    auto groupContainer = _pub.get().findDirectChild<GtObjectGroup*>(scopeId);

    if (!groupContainer)
    {
        gtError() << QObject::tr("Invalid process data!") 
                  << QObject::tr("Task group container was not found!");
        return false;
    }

    QDir dir(projectPath + QDir::separator() + "tasks" + QDir::separator() +
             scopeId);

    if (!dir.exists())
    {
        // no relevant directories found. not good not bad.
        return false;
    }

    // scope folder found. read entries and generate uninitialized task groups
    QDirIterator directories(dir.absolutePath(),
                             QDir::Dirs |
                             QDir::NoSymLinks |
                             QDir::NoDotAndDotDot);

    while (directories.hasNext())
    {
        directories.next();

        gtDebug().verbose() << "dir:" << directories.fileName();
        const QString& groupId = directories.fileName();

        auto retval = groupContainer->findDirectChild<GtTaskGroup*>(groupId);

        if (retval)
        {
           gtError() << QObject::tr("Task group '%1' already exists!")
                            .arg(groupId);
        }
        else
        {
            auto newGroup = new GtTaskGroup(groupId);
            groupContainer->appendChild(newGroup);
        }

    }

    return true;
}

bool
GtProcessData::Impl::initTaskGroup(const QString& groupId,
                                   const QString& projectPath,
                                   GtTaskGroup::SCOPE scope)
{
    QString scopeId = GtTaskGroup::scopeId(scope);

    if (scopeId.isEmpty())
    {
        gtError() << QObject::tr("Invalid task group scope");
        return false;
    }

    auto groupContainer = _pub.get().findDirectChild<GtObjectGroup*>(scopeId);

    if (!groupContainer)
    {
        gtError() << QObject::tr("Invalid process data!") 
                  << QObject::tr("Task group container was not found!");
        return false;
    }

    auto group = groupContainer->findDirectChild<GtTaskGroup*>(groupId);

    if (!group)
    {
        // task group not found! but its ok. we create a default one :)";
        return false;
    }

    if (!group->isInitialized())
    {
        // if group not already initialized. do it now!!! last chance my friend
        group->read(projectPath, scope);
    }

    _currentScope = scope;
    _currentGroupId = groupId;

    return true;
}

bool GtProcessData::Impl::initDefaultUserGroup(const QString& projectPath)
{
    // init default user
    QString sysUsername = GtTaskGroup::defaultUserGroupId();

    auto userGroups = _pub.get().findDirectChild<GtObjectGroup*>(
                GtTaskGroup::scopeId(GtTaskGroup::USER));

    if (!userGroups)
    {
        gtError() << QObject::tr("Invalid process data!") 
                  << QObject::tr("No user groups were found!");
        return false;
    }

    auto group = userGroups->findDirectChild<GtTaskGroup*>(sysUsername);

    if (group)
    {
        gtError() << QObject::tr("Default user group already exists!");
        return false;
    }


    group = new GtTaskGroup(sysUsername);

    if (!group->read(projectPath, GtTaskGroup::USER))
    {
        gtError() << QObject::tr("Could not initialize default user group!");
        delete group;
        return false;
    }

    userGroups->appendChild(group);

    _currentScope = GtTaskGroup::USER;
    _currentGroupId = sysUsername;

    gtInfo() << QObject::tr("Default user group successfully initialized");

    return true;
}

GtObjectGroup*
GtProcessData::Impl::groupContainer(GtTaskGroup::SCOPE scope) const
{
    QString scopeId = GtTaskGroup::scopeId(scope);

    if (scopeId.isEmpty())
    {
        gtError() << QObject::tr("Invalid task group scope");
        return nullptr;
    }

    return _pub.get().findDirectChild<GtObjectGroup*>(scopeId);
}

GtTaskGroup*
GtProcessData::Impl::currentTaskGroup() const
{
    GtObjectGroup* currentGroup = groupContainer(_currentScope);

    if (!currentGroup)
    {
        gtError() << QObject::tr("Current task group scope invalid!");
        return nullptr;
    }

    return currentGroup->findDirectChild<GtTaskGroup*>(_currentGroupId);
}

QList<GtTaskGroup*>
GtProcessData::Impl::userGroups()
{
    auto userGroups = _pub.get().findDirectChild<GtObjectGroup*>(
                GtTaskGroup::scopeId(GtTaskGroup::USER));

    if (!userGroups)
    {
        return {};
    }

    return userGroups->findDirectChildren<GtTaskGroup*>();
}

QList<GtTaskGroup*>
GtProcessData::Impl::customGroups()
{
    auto customGroups = _pub.get().findDirectChild<GtObjectGroup*>(
                GtTaskGroup::scopeId(GtTaskGroup::CUSTOM));

    if (!customGroups)
    {
        return {};
    }

    return customGroups->findDirectChildren<GtTaskGroup*>();
}
