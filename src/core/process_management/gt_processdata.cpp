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
#include "gt_project.h"

#include "gt_processdata.h"

class GtProcessData::Impl
{
public:
    std::reference_wrapper<GtProcessData> _pub;

    int _initialized = false;

    GtTaskGroup::SCOPE _currentScope = GtTaskGroup::USER;

    QString _currentGroupId;

    Impl (GtProcessData& pub) : _pub(pub) { }

    void readTaskGroups(const QString& projectPath, GtTaskGroup::SCOPE scope);

    bool initTaskGroup(const QString& groupId,
                       const QString& projectPath,
                       GtTaskGroup::SCOPE scope);

    void initDefaultUserGroup(const QString& projectPath);

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
        gtError() << "current task group not found!";
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
        gtError() << "current task group not found!";
        return nullptr;
    }

    return currentGroup->findDirectChild<GtTask*>(val);
}

GtTaskGroup*
GtProcessData::taskGroup()
{
    return m_pimpl->currentTaskGroup();
}

void
GtProcessData::init(const QString& projectPath)
{
    if (m_pimpl-> _initialized)
    {
        gtError() << "task group already initialized!";
        return;
    }

    // search all diectories
    m_pimpl->readTaskGroups(projectPath, GtTaskGroup::USER);
    m_pimpl->readTaskGroups(projectPath, GtTaskGroup::CUSTOM);


    // init last active task group
    QString sysUsername = qEnvironmentVariable("USER");

    if (sysUsername.isEmpty())
    {
        sysUsername = qEnvironmentVariable("USERNAME");
    }

    if (!m_pimpl->initTaskGroup(sysUsername, projectPath, GtTaskGroup::USER))
    {
        // last active task group not found. init default user group
        m_pimpl->initDefaultUserGroup(projectPath);
    }

    // initialization finished. Yippee-ki-yay, mot*********
    m_pimpl-> _initialized = true;
}

void
GtProcessData::save(const QString& projectPath)
{
    if (!m_pimpl-> _initialized)
    {
        gtError() << "cannot save task group. not initialized!";
        return;
    }

    foreach (auto* group, m_pimpl->userGroups())
    {
        group->save(projectPath, GtTaskGroup::USER);
    }

    foreach (auto* group, m_pimpl->customGroups())
    {
        group->save(projectPath, GtTaskGroup::CUSTOM);
    }
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

void
GtProcessData::Impl::readTaskGroups(const QString& projectPath,
                                    GtTaskGroup::SCOPE scope)
{
    QString scopeId = GtTaskGroup::scopeId(scope);

    if (scopeId.isEmpty())
    {
        gtError() << "invalid task group scope";
        return;
    }

    auto groupContainer = _pub.get().findDirectChild<GtObjectGroup*>(scopeId);

    if (!groupContainer)
    {
        gtError() << "invalid process data! task group container not found!";
        return;
    }

    QDir dir(projectPath + QDir::separator() + "tasks" + QDir::separator() +
             scopeId);

    if (!dir.exists())
    {
        // no relevant directories found. not good not bad.
        return;
    }

    // scope folder found. read entries and generate uninitialized task groups
    QDirIterator directories(dir.absolutePath(),
                             QDir::Dirs |
                             QDir::NoSymLinks |
                             QDir::NoDotAndDotDot);

    while (directories.hasNext())
    {
        directories.next();

        gtInfo() << "dir: " << directories.fileName();
        const QString& groupId = directories.fileName();

        auto retval = groupContainer->findDirectChild<GtTaskGroup*>(groupId);

        if (retval)
        {
            gtError() << "task group already exists! (" << groupId << ")";
        }
        else
        {
            auto newGroup = new GtTaskGroup(groupId);
            groupContainer->appendChild(newGroup);
        }

    }
}

bool
GtProcessData::Impl::initTaskGroup(const QString& groupId,
                                   const QString& projectPath,
                                   GtTaskGroup::SCOPE scope)
{
    QString scopeId = GtTaskGroup::scopeId(scope);

    if (scopeId.isEmpty())
    {
        gtError() << "invalid task group scope";
        return false;
    }

    auto groupContainer = _pub.get().findDirectChild<GtObjectGroup*>(scopeId);

    if (!groupContainer)
    {
        gtError() << "invalid process data! task group container not found!";
        return false;
    }

    auto group = groupContainer->findDirectChild<GtTaskGroup*>(groupId);

    if (!group)
    {
        // task group not found! but its ok. we create a default one :)";
        return false;
    }

    group->init(projectPath, scope);

    return true;
}

void
GtProcessData::Impl::initDefaultUserGroup(const QString& projectPath)
{
    // init default user
    QString sysUsername = GtTaskGroup::defaultUserGroupId();

    auto userGroups = _pub.get().findDirectChild<GtObjectGroup*>(
                GtTaskGroup::scopeId(GtTaskGroup::USER));

    if (!userGroups)
    {
        gtError() << "invalid process data! user groups not found!";
        return;
    }

    auto group = userGroups->findDirectChild<GtTaskGroup*>(sysUsername);

    if (group)
    {
        gtError() << "default user group already exists!";
        return;
    }


    group = new GtTaskGroup(sysUsername);

    if (!group->init(projectPath, GtTaskGroup::USER))
    {
        gtError() << "could not initialize default user group!";
        delete group;
        return;
    }

    userGroups->appendChild(group);

    _currentScope = GtTaskGroup::USER;
    _currentGroupId = sysUsername;

    gtInfo() << "default user group successfully initialized!";
}

GtObjectGroup*
GtProcessData::Impl::groupContainer(GtTaskGroup::SCOPE scope) const
{
    QString scopeId = GtTaskGroup::scopeId(scope);

    if (scopeId.isEmpty())
    {
        gtError() << "invalid task group scope";
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
        gtError() << "current task group scope invalid!";
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
