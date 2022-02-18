/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include <QDebug>

#include "gt_session.h"
#include "gt_project.h"
#include "gt_coreapplication.h"
#include "gt_coredatamodel.h"
#include "gt_logging.h"
#include "gt_h5filemanager.h"
#include "gt_externalizationsettings.h"

GtSession::GtSession(const QString& id) :
    m_currentProject(Q_NULLPTR)
{
    setObjectName(id);
    m_valid = fromJsonObject();
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

    if (project == Q_NULLPTR)
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

    project->resetAllExternalizedObjects(retval);

    return retval;
}

bool
GtSession::saveProjectData(GtProject* project)
{
    if (project == Q_NULLPTR)
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

    // force externalization or internalization
    if (gtExternalizationSettings->isExternalizationEnabled() &&
        gtExternalizationSettings->autoExternalizeOnSave())
    {
        // externalizes every child if its still fetched
        if (!project->externalizeAllChildren())
        {
            gtWarning() << "could not externalize all data!";
        }
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
        qWarning() << tr("WARNING") << ": "
                   << tr("roaming path not found!");
        return false;
    }

    QFile file(dir.absoluteFilePath(id + QStringLiteral(".json")));

    if (file.exists())
    {
        // file found -> nothing to do here
//        qWarning() << tr("WARNING") << ": "
//                   << tr("default session file already exists!");
        return true;
    }

    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not create session file!");
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
        qWarning() << tr("WARNING") << ": "
                   << tr("roaming path not found!");
        return false;
    }

    QFile file(dir.absoluteFilePath(source + QStringLiteral(".json")));

    if (!file.exists())
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("Session file not found!");
        return false;
    }

    QString targetFilename = target + QStringLiteral(".json");

//    qDebug() << "targetFilename = " << targetFilename;

    return file.copy(dir.absoluteFilePath(targetFilename));
}

GtSession::~GtSession()
{
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
GtSession::projects() const
{
    return findDirectChildren<GtProject*>();
}

QStringList
GtSession::projectIds() const
{
    QStringList retval;

    foreach (GtProject* project, projects())
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

//GtDataModel*
//GtSession::model()
//{
//    return m_model;
//}

bool
GtSession::setCurrentProject(const QString& id)
{
    GtProject* project = findProject(id);

    if (project == Q_NULLPTR)
    {
        qWarning() << tr("WARNING: ") <<
                      tr("could not set current project! project id not found");
        return false;
    }

    return setCurrentProject(project);
}

bool
GtSession::setCurrentProject(GtProject* project)
{
    if (project == Q_NULLPTR)
    {
        m_currentProject = Q_NULLPTR;
        return true;
    }

    if (!projects().contains(project))
    {
        qWarning() << tr("WARNING: ")
                   << tr("could not set current project!") << " "
                   << tr("project not found in session!");
        return false;
    }

    m_currentProject = project;

    // reset the temp file manager
    gtH5FileManager->reset(project, project->path());

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
        {
            setCurrentProject(project);
            return;
        }
    }

    setCurrentProject(Q_NULLPTR);
}

void
GtSession::addProject(GtProject* project)
{
    if (project != Q_NULLPTR)
    {
        project->acceptChanges();
        appendChild(project);
        toJsonObject();
    }
}

bool
GtSession::deleteProject(GtProject* project)
{
    if (project == Q_NULLPTR)
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
    if (project == Q_NULLPTR)
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
GtSession::toJsonObject()
{
    QFile file(sessionFilePath());

    if (!file.exists())
    {
        qWarning() << "session file not found!";
        return false;
    }

    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << "could not read session information!";
        return false;
    }

    QJsonObject jsobj;

    QJsonObject projectsJsn;

    foreach (GtProject* project, projects())
    {
        projectsJsn.insert(project->path(), project->isOpen());
    }

    jsobj.insert(QStringLiteral("projects"), projectsJsn);

    QJsonDocument saveDoc(jsobj);
    file.write(saveDoc.toJson());

    file.close();

    return true;
}

bool
GtSession::fromJsonObject()
{
    QFile file(sessionFilePath());

    if (!file.exists())
    {
        return false;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "could not read session information!";
        return false;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));

    QJsonObject json = doc.object();

    file.close();

    QJsonObject projects = json[QStringLiteral("projects")].toObject();

//    qDebug() << "projects:";
    for (auto const& e : projects.keys())
    {
//        qDebug() << "   |-> " << e;
        GtProject* project = new GtProject(e);

        if (!project->isValid() || findProject(project->objectName()))
        {
            qWarning() << "WARNING: " << "project " << project->objectName() <<
                          " already exists in session or is not valid!";
            gtError() << tr("Could not load project!")
                      << " (" << e << ")";
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
GtSession::sessionFilePath()
{
    QDir path(roamingPath());

    if (path.exists())
    {
        return path.absoluteFilePath(objectName() + QStringLiteral(".json"));
    }

    return QString();
}
