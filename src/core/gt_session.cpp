/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>

#include "gt_session.h"
#include "gt_project.h"
#include "gt_coreapplication.h"
#include "gt_logging.h"
#include "gt_algorithms.h"

GtSession::GtSession(const QString& id, QString sessionPath) :
    m_currentProject(nullptr)
{
    setObjectName(id);

    if (sessionPath.isEmpty())
    {
        sessionPath = sessionFilePath(id);
    }

    m_valid = fromJsonObject(std::move(sessionPath));
//    m_model = new GtDataModel(this);

//    connect(this, SIGNAL(dataChanged(GtObject*)), SLOT(onTreeDataChange()));
//    connect(this, SIGNAL(dataChanged(GtObject*,GtAbstractProperty*)),
//            SLOT(onTreeDataChange()));
}

GtSession::GtSession() : m_valid(false)
{

}

GtObjectList
GtSession::loadProjectData(GtProject* project)
{
    GtObjectList retval;

    if (!project)
    {
        return retval;
    }

    // module data
    GtObjectList moduleData = project->readModuleData();
    retval.append(moduleData);

    // process data
    retval.append(project->readProcessData());

    // label data
    retval.append(project->readLabelData(moduleData));

    return retval;
}

bool
GtSession::saveProjectData(GtProject* project)
{
    if (!project)
    {
        return false;
    }

    if (!project->isOpen())
    {
        return false;
    }

    if (!project->saveProjectOverallData())
    {
        return false;
    }

    if (!project->saveModuleData())
    {
        return false;
    }

    project->acceptChangesRecursively();

    gtInfo() << project->objectName() << tr("saved!");

    return true;
}

bool
GtSession::createDefault()
{
    return createEmptySession(QStringLiteral("default"));
}

bool
GtSession::createEmptySession(const QString& id)
{
    QString path = gtApp->roamingPath() + QDir::separator() +
                   QStringLiteral("session");

    QDir dir(path);

    if (!dir.exists())
    {
        auto dirCreated = QDir(gtApp->roamingPath()).mkdir("session");

        if (!dirCreated)
        {
            gtError() << tr("Cannot create session directory");
            return false;
        }
    }

    if (!dir.exists())
    {
        gtError() << tr("Session directory not found");
        return false;
    }

    QFile file(dir.absoluteFilePath(id + QStringLiteral(".json")));

    if (file.exists())
    {
        // file found -> nothing to do here
        return true;
    }

    if (!file.open(QIODevice::WriteOnly))
    {
        gtWarning().medium() << tr("Could not create session file!");
        return false;
    }

    file.write(QJsonDocument(QJsonObject()).toJson());

    file.close();

    return true;
}

bool
GtSession::duplicateSession(const QString& source, const QString& target)
{
    QString path = gtApp->roamingPath() + QDir::separator() +
                   QStringLiteral("session");

    QDir dir(path);

    if (!dir.exists())
    {
        gtWarning().medium() << tr("WARNING") << ": "
                             << tr("roaming path not found!");
        return false;
    }

    QFile file(dir.absoluteFilePath(source + QStringLiteral(".json")));

    if (!file.exists())
    {
        gtWarning().medium() << tr("Session file not found!");
        return false;
    }

    QString targetFilename = target + QStringLiteral(".json");

    return file.copy(dir.absoluteFilePath(targetFilename));
}

GtProject*
GtSession::currentProject() const
{
    return m_currentProject;
}

GtProject*
GtSession::findProject(const QString& id)
{
    return findDirectChild<GtProject*>(id);
}

QList<GtProject*>
GtSession::projects()
{
    return findDirectChildren<GtProject*>();
}

QList<GtProject const*>
GtSession::projects() const
{
    return findDirectChildren<GtProject const*>();
}

QStringList
GtSession::projectIds() const
{
    QStringList retval;

    for (GtProject const* project : projects())
    {
        retval << project->objectName();
    }

    return retval;
}

bool
GtSession::isValid()
{
    return m_valid;
}

bool
GtSession::save()
{
    return toJsonObject();
}

bool
GtSession::setCurrentProject(const QString& id)
{
    GtProject* project = findProject(id);

    if (!project)
    {
        gtWarning().medium() << tr("Could not set current project! (project id not found)");
        return false;
    }

    return setCurrentProject(project);
}

bool
GtSession::setCurrentProject(GtProject* project)
{
    if (!project)
    {
        m_currentProject = nullptr;
        return true;
    }

    if (!projects().contains(project))
    {
        gtWarning().medium() << tr("Could not set current project!")
                             << tr("(Project not found in session)");
        return false;
    }

    m_currentProject = project;

    return true;
}

void
GtSession::switchCurrentProject()
{
    QList<GtProject*> projTmp = projects();

    if (projTmp.contains(m_currentProject))
    {
        projTmp.removeOne(m_currentProject);
    }

    foreach (GtProject* project, projTmp)
    {
        if (project->isOpen())
        { // cppcheck-suppress useStlAlgorithm
            setCurrentProject(project);
            return;
        }
    }

    setCurrentProject(nullptr);
}

void
GtSession::addProject(GtProject* project)
{
    if (project)
    {
        project->acceptChanges();
        appendChild(project);
        toJsonObject();
    }
}

bool
GtSession::deleteProject(GtProject* project)
{
    if (!project)
    {
        gtDebug() << tr("Cannot delete project!")
                  << " project == NULL";
        return false;
    }

    if (!projects().contains(project))
    {
        gtDebug() << tr("Cannot delete project!")
                  << " " << tr("Project is not inside current session.");
        return false;
    }

    if (project->isOpen())
    {
        gtWarning() << tr("Cannot delete an open project!");
        return false;
    }

    delete project;

    toJsonObject();

    return true;
}

int
GtSession::projectIndex(GtProject* project)
{
    if (!project)
    {
        return false;
    }

    return projects().indexOf(project);
}

QString
GtSession::roamingPath()
{
    return GtCoreApplication::roamingPath() + QDir::separator() +
           QStringLiteral("session");
}

bool 
GtSession::hasDefaultSession()
{
    const QString path = gtApp->roamingPath() + QDir::separator() +
                         QStringLiteral("session") + QDir::separator() +
                         "default.json";

    return QFileInfo::exists(path);
}

bool
GtSession::toJsonObject()
{
    QFile file(sessionFilePath(objectName()));

    if (!file.exists())
    {
        gtWarning().medium() << tr("Session file not found!");
        return false;
    }

    if (!file.open(QIODevice::WriteOnly))
    {
        gtWarning().medium() << tr("Could not read session information!");
        return false;
    }

    QJsonObject jsobj;

    QJsonObject projectsJsn;

    foreach (GtProject* project, projects())
    {
        projectsJsn.insert(project->path(), project->ignoringIrregularities());
    }

    jsobj.insert(QStringLiteral("projects"), projectsJsn);

    QJsonDocument saveDoc(jsobj);
    file.write(saveDoc.toJson());

    file.close();

    return true;
}

bool
GtSession::fromJsonObject(const QString& sessionPath)
{
    QFile file(sessionPath);

    if (!file.exists())
    {
        return false;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        gtWarning().medium() << tr("Could not read session information!");
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));

    QJsonObject json = doc.object();

    file.close();

    QJsonObject projects = json[QStringLiteral("projects")].toObject();

    for (auto it = projects.begin(); it != projects.end(); ++it) {
        GtProject* project = new GtProject(it.key());
        project->setProperty("tmp_ignoreIrregularities", it.value().toBool());

        if (!project->isValid() || findProject(project->objectName()))
        {
            gtWarning().medium()
                << tr("Project '%1' already exists in session or is not valid!")
                       .arg(project->objectName());
            gtError() << tr("Could not load project '%1'!").arg(it.key());
            delete project;
        }
        else
        {
            addProject(project);
        }
    }

    return true;
}

void
GtSession::onTreeDataChange()
{
    gtDebug() << "session changed!";
}

QString
GtSession::sessionFilePath(const QString& sessionID)
{
    QDir path(roamingPath());

    if (path.exists())
    {
        return path.absoluteFilePath(sessionID + QStringLiteral(".json"));
    }

    return QString();
}
