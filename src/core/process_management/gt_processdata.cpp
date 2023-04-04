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
#include "gt_qtutilities.h"

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

    QList<const GtTaskGroup*> userGroups() const;

    QList<const GtTaskGroup*> customGroups() const;

    /**
     * @brief Returns all task groups for given scope
     * @param scope
     * @return
     */
    QList<const GtTaskGroup*> groups(GtTaskGroup::SCOPE scope) const;

    /**
     * @brief Returns identification strings of all task groups for given scope
     * @param scope
     * @return
     */
    QStringList groupIds(GtTaskGroup::SCOPE scope) const;

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

QList<GtTask const*>
GtProcessData::currentProcessList() const
{
    GtTaskGroup const* currentGroup = m_pimpl->currentTaskGroup();
    if (!currentGroup)
    {
        gtError() << tr("Current task group not found!");
        return {};
    }

    return currentGroup->findDirectChildren<GtTask const*>();
}

QList<GtTask const*>
GtProcessData::processList(const QString& groupId) const
{
    auto const groups = m_pimpl->userGroups() + m_pimpl->customGroups();

    auto iter = std::find_if(std::begin(groups), std::end(groups),
                             [&](GtTaskGroup const* taskGroup){
        return taskGroup->objectName() == groupId;
    });

    if (iter == std::end(groups))
    {
        return {};
    }

    return (*iter)->findDirectChildren<GtTask const*>();
}

GtTask*
GtProcessData::findProcess(const QString& name)
{
    GtTaskGroup* currentGroup = m_pimpl->currentTaskGroup();

    if (!currentGroup)
    {
        gtError() << tr("Current task group not found!");
        return nullptr;
    }

    return currentGroup->findDirectChild<GtTask*>(name);
}

const GtTask*
GtProcessData::findProcess(const QString& name) const
{
    return const_cast<GtProcessData*>(this)->findProcess(name);
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
    if (m_pimpl->_initialized)
    {
        gtError() << tr("Process data was already initialized!");
        return false;
    }

    // search all diectories
    if (!m_pimpl->readTaskGroups(projectPath, GtTaskGroup::USER))
    {
        gtError() << tr("Unable to read user task groups!");
        return false;
    }

    if (!m_pimpl->readTaskGroups(projectPath, GtTaskGroup::CUSTOM))
    {
        gtError() << tr("Unable to read custom task groups!");
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
    m_pimpl->_initialized = true;

    return true;
}

bool
GtProcessData::switchCurrentTaskGroup(const QString& taskGroupId,
                                      GtTaskGroup::SCOPE scope,
                                      const QString& projectPath)
{
    if (!m_pimpl->_initialized)
    {
        gtError() << tr("Process data is not initialized!") << tr("Call init() first!");
        return false;
    }

    return m_pimpl->initTaskGroup(taskGroupId, projectPath, scope);
}

bool
GtProcessData::save(const QString& projectPath) const
{
    if (!m_pimpl->_initialized)
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

GtTaskGroup*
GtProcessData::createNewTaskGroup(const QString& taskGroupId,
                                  GtTaskGroup::SCOPE scope,
                                  const QString& projectPath)
{
    GtObjectGroup* groupContainer = m_pimpl->groupContainer(scope);

    if (!groupContainer)
    {
        gtError().medium() << QObject::tr("task group scope invalid!");
        return nullptr;
    }

    auto group = groupContainer->findDirectChild<GtTaskGroup*>(taskGroupId);

    if (group)
    {
        gtError() << QObject::tr("task group already exists!");
        return nullptr;
    }

    auto newGroup = std::make_unique<GtTaskGroup>(taskGroupId);

    if (!newGroup->read(projectPath, scope))
    {
        gtError() << QObject::tr("Could not initialize default group!");
        return nullptr;
    }

    groupContainer->appendChild(newGroup.get());

    return newGroup.release();
}

bool
GtProcessData::renameTaskGroup(const QString& taskGroupId,
                               const QString& taskGroupIdNew,
                               GtTaskGroup::SCOPE scope,
                               const QString& projectPath)
{
    if (!m_pimpl->_initialized)
    {
        gtError().medium() << tr("Process data is not initialized!") <<
                     tr("Call init() first!");
        return false;
    }

    GtObjectGroup* groupContainer = m_pimpl->groupContainer(scope);

    if (!groupContainer)
    {
        gtError().medium() << QObject::tr("task group scope invalid!");
        return false;
    }

    auto group = groupContainer->findDirectChild<GtTaskGroup*>(taskGroupId);

    if (!group)
    {
        gtError().medium() << QObject::tr("task group not found!");
        return false;
    }

    if (m_pimpl->groupIds(scope).contains(taskGroupIdNew))
    {
        gtError() << QObject::tr("new task group id already used!");
        return false;
    }

    const QString oldPath = GtTaskGroup::groupPath(projectPath, scope,
                                                   taskGroupId);
    const QString newPath = GtTaskGroup::groupPath(projectPath, scope,
                                                   taskGroupIdNew);

    QDir oldDir(oldPath);
    QDir newDir(newPath);

    if (!oldDir.exists())
    {
        gtError() << QObject::tr("task group file path not found!");
        return false;
    }

    if (newDir.exists())
    {
        gtError() << QObject::tr("new task group file path already exists!");
        return false;
    }

    if (!oldDir.cdUp())
    {
        gtError() << QObject::tr("task group scope path not found!");
        return false;
    }

    if (!oldDir.rename(taskGroupId, taskGroupIdNew))
    {
        gtError() << QObject::tr("could not rename task group path!");
        return false;
    }

    group->setObjectName(taskGroupIdNew);

    return true;
}

QStringList
GtProcessData::userGroupIds() const
{
    return m_pimpl->groupIds(GtTaskGroup::USER);
}

QStringList
GtProcessData::customGroupIds() const
{
    return m_pimpl->groupIds(GtTaskGroup::CUSTOM);
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
        return true;
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

bool
GtProcessData::Impl::initDefaultUserGroup(const QString& projectPath)
{
    // init default user
    QString sysUsername = GtTaskGroup::defaultUserGroupId();

    auto group = _pub.get().createNewTaskGroup(sysUsername,
                                               GtTaskGroup::USER,
                                               projectPath);

    if (!group) return false;

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

QList<const GtTaskGroup*>
GtProcessData::Impl::userGroups() const
{
    return groups(GtTaskGroup::USER);
}

QList<const GtTaskGroup*>
GtProcessData::Impl::customGroups() const
{
    return groups(GtTaskGroup::CUSTOM);
}

QList<const GtTaskGroup*>
GtProcessData::Impl::groups(GtTaskGroup::SCOPE scope) const
{
    auto customGroups = _pub.get().findDirectChild<GtObjectGroup*>(
                GtTaskGroup::scopeId(scope));

    if (!customGroups)
    {
        return {};
    }

    return gt::container_const_cast(
                customGroups->findDirectChildren<GtTaskGroup*>());
}

QStringList
GtProcessData::Impl::groupIds(GtTaskGroup::SCOPE scope) const
{
    QStringList retval;

    foreach (auto* group, groups(scope))
    {
        retval << group->objectName();
    }

    return retval;
}
