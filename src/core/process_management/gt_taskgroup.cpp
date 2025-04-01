/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 12.10.2022
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDomDocument>
#include <QDomElement>

#include "gt_algorithms.h"
#include "gt_logging.h"
#include "gt_taskgroup.h"
#include "gt_processfactory.h"
#include "gt_task.h"
#include "gt_objectfactory.h"
#include "gt_xmlutilities.h"
#include "internal/gt_platformspecifics.h"

static const char* S_INDEX_FILE_NAME = "index.json";
static const char* S_TASK_FILE_EXT = ".gttask";

class GtTaskGroup::Impl
{
public:
    std::reference_wrapper<GtTaskGroup> _pub;

    GtBoolProperty _initialized{"initialized", tr("initialized"),
                tr("Task Group is initialized."), false};

    explicit Impl (GtTaskGroup& pub) : _pub(pub) { }

    bool updateIndexFile(const QString& projectPath,
                         const GtTaskGroup::SCOPE scope) const;

    QString path(const QString& projectPath,
                 const GtTaskGroup::SCOPE scope) const;

    std::unique_ptr<GtTask> createTaskFromFile(const QString& filePath) const;

    bool saveTaskToFile(const GtTask* task, const QString& groupPath) const;

    static bool createSubFolder(QDir& dir,
                         const QString& subFolderId);

    static bool initFileStructure(const QString& projectPath,
                                  const GtTaskGroup::SCOPE scope,
                                  const QString& groupId);

    static bool appendTaskToIndex(const QString& projectPath,
                                  const GtTaskGroup::SCOPE scope,
                                  const QString& groupId,
                                  const QString& taskUuid);

};

GtTaskGroup::GtTaskGroup() : m_pimpl{std::make_unique<Impl>(*this)}
{
    setFactory(gtObjectFactory);

    m_pimpl->_initialized.hide();
    registerProperty(m_pimpl->_initialized);
}

GtTaskGroup::GtTaskGroup(const QString& id) : GtTaskGroup()
{
    setObjectName(id);
}

GtTaskGroup::GtTaskGroup(const QString& id, bool initialized) :
    GtTaskGroup(id)
{
    m_pimpl->_initialized = initialized;
}

GtTaskGroup::~GtTaskGroup() = default;

bool
GtTaskGroup::read(const QString& projectPath,
                  const SCOPE scope)
{
    if (m_pimpl-> _initialized)
    {
        gtError() << tr("Task group already initialized!");
        return false;
    }

    QDir dir(m_pimpl->path(projectPath, scope));

    gtDebug().medium() << tr("Task group path") <<  '=' << dir.absolutePath();

    // read json file
    QFile idxFile(dir.absoluteFilePath(S_INDEX_FILE_NAME));

    if (!idxFile.exists())
    {
        // index file not found. empty or new task group
        m_pimpl-> _initialized = true;
        return true;
    }

    // index file found. create tasks from mementos
    if (!idxFile.open(QIODevice::ReadOnly))
    {
        gtError() << tr("Failed to open index file!");
        return false;
    }

    QByteArray data = idxFile.readAll();

    QJsonDocument doc(QJsonDocument::fromJson(data));

    QJsonObject json = doc.object();

    // json doc created. close file
    idxFile.close();

    // active tasks
    QJsonArray activeTasks = json[QStringLiteral("active")].toArray();

    for (const auto& e : qAsConst(activeTasks))
    {
        auto newTask = m_pimpl->createTaskFromFile(
                    dir.absoluteFilePath(e.toString() + S_TASK_FILE_EXT));

        if (newTask)
        {
            gtDebug().medium().nospace() << "new task created ("
                                         << newTask->uuid() << ")";
            appendChild(newTask.release());
        }
    }

    acceptChangesRecursively();

    m_pimpl-> _initialized = true;
    return true;
}

bool
GtTaskGroup::save(const QString& projectPath,
                  const GtTaskGroup::SCOPE scope) const
{
    if (!m_pimpl-> _initialized)
    {
        gtDebug().nospace() << "Save procedure not needed. Group not initialized! (" <<
                     objectName() << ")";
        return true;
    }

    // init full file structure for given task group
    if (!m_pimpl->initFileStructure(projectPath, scope, objectName()))
    {
        return false;
    }

    QString groupPath = m_pimpl->path(projectPath, scope);
    assert(!groupPath.isEmpty());

    // get the list of the existing task files
    QStringList taskFilesToRemove = QDir{groupPath}.entryList(QDir::Files);
    taskFilesToRemove.removeOne(S_INDEX_FILE_NAME);

    // externalize tasks
    const auto& tasks = findDirectChildren<GtTask*>();
    for (const GtTask* task : tasks)
    {
        if (!m_pimpl->saveTaskToFile(task, m_pimpl->path(projectPath, scope)))
        {
            return false;
        }

        taskFilesToRemove.removeOne(task->uuid() + S_TASK_FILE_EXT);
    }

    // remove task files that are no longer known by the task group
    for (const auto& fileName : qAsConst(taskFilesToRemove))
    {
        QFile{groupPath + QDir::separator() + fileName}.remove();
    }

    // final step: update index file
    return m_pimpl->updateIndexFile(projectPath, scope);
}

bool
GtTaskGroup::isInitialized() const
{
    return m_pimpl-> _initialized;
}

QString
GtTaskGroup::scopeId(const SCOPE scope)
{
    QString retval;

    switch (scope)
    {
    case GtTaskGroup::USER:
        retval = "_user";
        break;
    case GtTaskGroup::CUSTOM:
        retval = "_custom";
        break;
    default:
        gtError() << tr("Invalid task group scope");
        return {};
    }

    return retval;
}

QString
GtTaskGroup::defaultUserGroupId()
{
    auto id = QString::fromStdString(gt::detail::systemUsername());
    if (id.isEmpty()) id = "undefined";

    return id;
}

QString
GtTaskGroup::scopePath(const QString& projectPath,
                       const SCOPE scope)
{
    QString scopeId = GtTaskGroup::scopeId(scope);

    if (scopeId.isEmpty())
    {
        gtError() << tr("Invalid task group scope");
        return {};
    }

    return projectPath + QDir::separator() + "tasks" +
            QDir::separator() + scopeId;
}

QString
GtTaskGroup::groupPath(const QString& projectPath,
                       const SCOPE scope,
                       const QString& groupId)
{
    QString scopePath = GtTaskGroup::scopePath(projectPath, scope);

    if (scopePath.isEmpty())
    {
        return {};
    }

    return scopePath + QDir::separator() + groupId;
}

bool
GtTaskGroup::saveTaskElementToFile(const QString& projectPath,
                                   const SCOPE scope,
                                   const QString& groupId,
                                   const QDomElement& taskElement)
{
    QString taskUuid = taskElement.attribute("uuid");

    if (taskUuid.isEmpty())
    {
        gtError() << tr("Task uuid not found! (Task element corrupted)");
        return false;
    }

    if (!GtTaskGroup::Impl::initFileStructure(projectPath, scope, groupId))
    {
        return false;
    }

    const QString fileName = groupPath(projectPath, scope, groupId) + QDir::separator()
            + taskUuid + S_TASK_FILE_EXT;

    gtDebug().medium().nospace() << "writing task file (" << fileName << ")...";

    QFile taskFile(fileName);

    if (taskFile.exists())
    {
        gtError() << tr("File already exists:") << taskFile.fileName();
        return false;
    }

    if (!taskFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gtError() << tr("Could not open file:") << taskFile.fileName();
        return false;
    }

    QTextStream out(&taskFile);

    QDomDocument doc;

    // element is the QDomElement object
    QString str;
    QTextStream stream(&str, QIODevice::WriteOnly);
    taskElement.save(stream, 2); // stored the content of QDomElement to stream

    doc.setContent(str.toUtf8());

    gtDebug().medium() << "writing doc...";
    out << doc.toString();

    taskFile.close();

    // append task to index
    if (!GtTaskGroup::Impl::appendTaskToIndex(projectPath, scope, groupId,
                                              taskUuid))
    {
        gtError() << tr("Could not append task to index file!");
        return false;
    }

    return true;
}

bool
GtTaskGroup::Impl::initFileStructure(const QString& projectPath,
                                     const GtTaskGroup::SCOPE scope,
                                     const QString& groupId)
{
    QDir dir(projectPath);

    if (!dir.exists())
    {
        gtError().nospace() << tr("Project path not found! (%1)").arg(projectPath);
        return false;
    }

    if (!createSubFolder(dir, "tasks"))
        return false;

    if (!createSubFolder(dir, GtTaskGroup::scopeId(scope)))
        return false;

    // check group id folder. if not available, create new.
    if (!createSubFolder(dir, groupId))
        return false;

    return true;
}

bool
GtTaskGroup::Impl::appendTaskToIndex(const QString& projectPath,
                                     const SCOPE scope,
                                     const QString& groupId,
                                     const QString& taskUuid)
{
    QDir dir(GtTaskGroup::groupPath(projectPath, scope, groupId));

    // check for existance
    QFile idxFile(dir.absoluteFilePath(S_INDEX_FILE_NAME));

    QJsonObject jroot;
    QJsonArray jarray;

    if (idxFile.exists())
    {
        // open index file
        if (!idxFile.open(QIODevice::ReadWrite))
        {
            gtError() << QObject::tr("Could not open index file!");
            return false;
        }

        QByteArray data = idxFile.readAll();
        QJsonDocument doc(QJsonDocument::fromJson(data));

        idxFile.resize(0);
        jroot = doc.object();
        jarray = jroot["active"].toArray();
    }
    else
    {
        // open index file
        if (!idxFile.open(QIODevice::WriteOnly))
        {
            gtError() << QObject::tr("Could not create index file!");
            return false;
        }
    }

    if (!jarray.contains(taskUuid))
    {
        jarray.push_back(taskUuid);
    }

    jroot.insert(QStringLiteral("active"), jarray);

    QJsonDocument saveDoc(jroot);
    idxFile.write(saveDoc.toJson());

    idxFile.close();

    // we are happy
    return true;
}

bool
GtTaskGroup::Impl::createSubFolder(QDir& dir, const QString& subFolderId)
{
    if (subFolderId.isEmpty())
    {
        gtError() << QObject::tr("Folder id is empty!");
        return false;
    }

    if (!dir.cd(subFolderId))
    {
        if (!dir.mkdir(subFolderId))
        {
            gtError() << QObject::tr("Could not create folder! (%1)").arg(subFolderId);
            return false;
        }

        return dir.cd(subFolderId);
    }

    // nothing to do, satisfaction
    return true;
}

bool
GtTaskGroup::Impl::updateIndexFile(const QString &projectPath,
                             const GtTaskGroup::SCOPE scope) const
{
    QDir dir(path(projectPath, scope));

    gtDebug().medium() << "task group index file: " <<
                 dir.absoluteFilePath(S_INDEX_FILE_NAME);

    // check for existance
    QFile idxFile(dir.absoluteFilePath(S_INDEX_FILE_NAME));

    // open index file
    if (!idxFile.open(QIODevice::WriteOnly))
    {
        gtError() << QObject::tr("Could not create index file!");
        return false;
    }

    QJsonObject jroot;
    QJsonArray jarray;

    // write tasks
    foreach (GtTask* task, _pub.get().findDirectChildren<GtTask*>())
    {
        jarray.push_back(task->uuid());
    }

    jroot.insert(QStringLiteral("active"), jarray);

    QJsonDocument saveDoc(jroot);
    idxFile.write(saveDoc.toJson());

    idxFile.close();

    // we are happy
    return true;
}

QString
GtTaskGroup::Impl::path(const QString& projectPath,
                      const GtTaskGroup::SCOPE scope) const
{
    return _pub.get().groupPath(projectPath, scope, _pub.get().objectName());
}

std::unique_ptr<GtTask>
GtTaskGroup::Impl::createTaskFromFile(const QString& filePath) const
{
    QFile taskFile(filePath);
    GtTask* retval = nullptr;

    if (!taskFile.exists())
    {
        // task file not found
        gtError() << QObject::tr("Task file not found (%1)").arg(filePath);
        return nullptr;
    }

    // index file found. create tasks from mementos
    QDomDocument document;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!gt::xml::readDomDocumentFromFile(taskFile, document, true, &errorStr,
                                          &errorLine, &errorColumn))
    {
        gtError() << QObject::tr("Could not open task file (%1)").arg(filePath);
        return nullptr;
    }

    QDomElement root = document.documentElement();

    GtObjectMemento memento(root);
    if (memento.isNull())
    {
        gtError() << QObject::tr("Could not parse task file (%1)").arg(filePath);
        return nullptr;
    }

    auto obj = memento.restore(gtProcessFactory);

    if (obj)
    {
        retval = qobject_cast<GtTask*>(obj);
        if (!retval)
        {
            gtError() << QObject::tr("Invalid task file (%1)").arg(filePath);
            delete obj;
        }
    }

    return std::unique_ptr<GtTask>(retval);
}

bool
GtTaskGroup::Impl::saveTaskToFile(const GtTask* task, const QString& groupPath) const
{
    QString taskFile(groupPath + QDir::separator() + task->uuid() +
                   S_TASK_FILE_EXT);


    QDomDocument doc;
    doc.setContent(task->toMemento().toByteArray());

    if (!gt::xml::writeDomDocumentToFile(taskFile, doc, true))
    {
        gtError() << QObject::tr("Could not open file (%1)").arg(taskFile);
        return false;
    }

    return true;
}
