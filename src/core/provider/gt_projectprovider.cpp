/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_projectprovider.h"
#include "gt_project.h"
#include "gt_logging.h"
#include "gt_coredatamodel.h"
#include "gt_objectlinkproperty.h"
#include "gt_propertyconnection.h"
#include "gt_footprint.h"
#include "gt_algorithms.h"

#include <QFile>
#include <QDir>
#include <QUuid>
#include <QDomDocument>

#include <algorithm>

GtProjectProvider::GtProjectProvider(QObject* parent) : QObject(parent),
    m_project(nullptr)
{
    //    m_pName = QStringLiteral("Hello Project");
    //    m_pPath = QStringLiteral("D:/temp/test");
}

GtProjectProvider::GtProjectProvider(const QString& filename,
                                     QObject* parent) :
    QObject(parent),
    m_project(nullptr)
{
    QFile file(filename);

    QFileInfo info(file);

    setSource(new GtProject(info.absolutePath()));
}

GtProjectProvider::GtProjectProvider(GtProject* project, QObject* parent) :
    QObject(parent)
{
    setSource(project);
}

void
GtProjectProvider::setSource(GtProject* project)
{
    if (project)
    {
        m_project = project;
        mapFromSource();
    }
}

GtProject*
GtProjectProvider::project()
{
    mapToSource();
    return m_project;
}

GtProject*
GtProjectProvider::duplicateProject(const QString& newId,
                                    const QString& newPath)
{
    if (!m_project)
    {
        return nullptr;
    }

    QString tmpId = m_pName;
    QString tmpPath = m_pPath;
    GtProject* tmpProject = m_project;

    m_pName = newId;
    m_pPath = newPath;
    m_project = nullptr;

    GtProject* retval = project();

    // copy objects
    foreach (GtObject* obj, tmpProject->findDirectChildren<GtObject*>())
    {
        GtObject* copiedObj = obj->clone();

        if (!copiedObj)
        {
            gtError() << tr("Could not copy object!");
            continue;
        }

        retval->appendChild(copiedObj);
    }

    QList<GtObject*> objs = retval->findChildren<GtObject*>();

    // reset uuids and store mapped uuid informations
    QMap<QString, QString> uuidMap;

    foreach (GtObject* obj, objs)
    {
        resetUuid(obj, uuidMap);
    }

    // modify copied object links
    foreach (GtObject* obj, objs)
    {
        foreach (GtObjectLinkProperty* prop, objectLinkProperties(obj))
        {
            if (uuidMap.contains(prop->getVal()))
            {
                prop->setVal(uuidMap.value(prop->getVal()));
            }
        }
    }

    // modify copied property connections
    foreach (GtObject* obj, objs)
    {
        GtPropertyConnection* propCon =
                qobject_cast<GtPropertyConnection*>(obj);

        if (propCon)
        {
            if (uuidMap.contains(propCon->sourceUuid()))
            {
                propCon->setSourceUuid(uuidMap.value(propCon->sourceUuid()));
            }

            if (uuidMap.contains(propCon->targetUuid()))
            {
                propCon->setTargetUuid(uuidMap.value(propCon->targetUuid()));
            }

            propCon->makeConnection();
        }
    }

    QDir oldDirectory(tmpPath);
    QDir newDirectory(newPath);

    // moidify uuids and copy ini file
    QFile iniFile(oldDirectory.absoluteFilePath(QStringLiteral("project.ini")));

    if (iniFile.exists())
    {
        if (iniFile.open(QFile::ReadOnly | QFile::Text))
        {
            QTextStream in(&iniFile);
            QString content = in.readAll();

            gt::for_each_key (uuidMap, [&](const QString& e)
            {
                QString oldUuid = e;
                oldUuid.remove(QStringLiteral("{"));
                oldUuid.remove(QStringLiteral("}"));

                QString newUuid = uuidMap.value(e);
                newUuid.remove(QStringLiteral("{"));
                newUuid.remove(QStringLiteral("}"));

                content.replace(oldUuid, newUuid);
            });

            QFile newIniFile(newDirectory.absoluteFilePath(
                                 QStringLiteral("project.ini")));

            if (newIniFile.open(QFile::WriteOnly | QFile::Text))
            {
                QTextStream out(&newIniFile);
                out << content;
            }
        }
    }

        // copy template files
    QDir oldPostDir(oldDirectory.absoluteFilePath(QStringLiteral("post")));
    QDir newPostDir(newDirectory.absoluteFilePath(QStringLiteral("post")));

    if (oldPostDir.exists())
    {
        // create new post dir if not exists
        if (!newPostDir.exists())
        {
            if (!newPostDir.mkpath(newPostDir.absolutePath()))
            {
                gtError() << "could not create copy of post directory";
                delete retval;
                return nullptr;
            }
        }

        QStringList postEntries =
                oldPostDir.entryList(QStringList() << QStringLiteral("*.post"));

        foreach (const QString& str, postEntries)
        {
            QFile postFile(oldPostDir.absoluteFilePath(str));

            postFile.copy(newPostDir.absoluteFilePath(str));
        }
    }


    // TODO: modify uuids and copy sqlite file

    retval->acceptChangesRecursively();

    m_pName = tmpId;
    m_pPath = tmpPath;
    m_project = tmpProject;

    return retval;
}

std::unique_ptr<GtProject>
GtProjectProvider::duplicateExistingProject(const QDir& projectPath,
                                            const QDir& newProjectPath,
                                            const QString& newProjectName)
{
    // exluding backups
    auto status = gt::project::copyProjectData(projectPath, newProjectPath,
                                               gt::project::ForceOverwrite);

    if (status != gt::filesystem::CopyStatus::Success)
    {
        return nullptr;
    }

    // update project meta data. and of backups?
    GtProject::ProjectMetaData md;
    md.projectName = newProjectName;

    if (!GtProject::updateProjectMetaData(newProjectPath, md))
    {
        return nullptr;
    }

    auto newProjectFile = newProjectPath.path() + QDir::separator() +
                          GtProject::mainFilename();

    GtProjectProvider provider(newProjectFile);
    return std::unique_ptr<GtProject>(provider.project());
}

bool
GtProjectProvider::apply()
{
    if (!m_project)
    {
        gtError() << tr("could not apply changes") << QStringLiteral("!")
                  << QStringLiteral("(project == NULL)");
        return false;
    }

    if (m_project->isOpen())
    {
        return false;
    }

    if (!modifyFiles())
    {
        return false;
    }

    m_project->setModuleIds(m_pModules);

    return true;
}

const QString&
GtProjectProvider::projectName()
{
    return m_pName;
}

void
GtProjectProvider::setProjectName(const QString& val)
{
    if (val.isEmpty())
    {
        return;
    }

    m_pName = val;
}

const QString&
GtProjectProvider::projectPath()
{
    return m_pPath;
}

void
GtProjectProvider::setProjectPath(const QString& path)
{
    if (path.isEmpty())
    {
        return;
    }

    m_pPath = path;
}

const QStringList&
GtProjectProvider::projectModules()
{
    return m_pModules;
}

void
GtProjectProvider::setProjectModules(const QStringList& val)
{
    m_pModules = val;
}

bool
GtProjectProvider::projectFileExists(const QString& path)
{
    const QString mainFilename = path + QDir::separator() +
                                 GtProject::mainFilename();

    QFile file(mainFilename);

    // check existing project file
    return file.exists();
}

bool
GtProjectProvider::projectExistsInSession(const QString& id)
{
    auto ids = gtDataModel->projectIds();
    return std::find(std::begin(ids), std::end(ids), id) != std::end(ids);
}

void GtProjectProvider::mapFromSource()
{
    if (m_project)
    {
        m_pName = m_project->objectName();
        m_pPath = m_project->path();
        m_pModules = m_project->moduleIds();
    }
}

void
GtProjectProvider::mapToSource()
{
    if (m_project)
    {
        if (m_project->isOpen())
        {
            return;
        }

        m_project->setObjectName(m_pName);
    }
    else
    {
        //        qDebug() << "selected modules: " << m_pModules;

        if (generateFiles())
        {
            m_project = new GtProject(m_pPath);
            m_project->setObjectName(m_pName);

            if (!m_project->isValid())
            {
                delete m_project;
            }

            //            m_project->setModuleIds(m_pModules);
        }
    }
}

bool
GtProjectProvider::generateFiles()
{
    if (m_pName.isEmpty() || m_pPath.isEmpty())
    {
        gtError() << "project name or project path is empty!";
        return false;
    }

    if (!generateMainProjectFile())
    {
        return false;
    }

    if (!generateModuleFiles())
    {
        return false;
    }

    return true;
}

bool
GtProjectProvider::modifyFiles()
{
    if (!m_project)
    {
        return false;
    }

    if (m_project->isOpen())
    {
        return false;
    }

    if (!modifyMainProjectFile())
    {
        return false;
    }

    foreach (const QString& modStrItem, m_pModules)
    {
        const QString filename = moduleFilename(modStrItem.toLower());

        if (filename.isEmpty())
        {
            gtError() << tr("could not generate module file")
                      << QStringLiteral("! ") << tr("empty file name")
                      << QStringLiteral("!");
            return false;
        }

        if (!QFile(filename).exists())
        {
            generateModuleFile(modStrItem.toLower());
        }
    }

    return true;
}

bool
GtProjectProvider::generateMainProjectFile()
{
    if (m_pName.isEmpty() || m_pPath.isEmpty())
    {
        gtError() << "project name or project path is empty!";
        return false;
    }

    const QString mainFilename = m_pPath + QDir::separator() +
                                 GtProject::mainFilename();

    QFile file(mainFilename);

    // check existing project file
    if (projectFileExists(m_pPath))
    {
        gtError() << "project file already exists in directory!";
        return false;
    }

    if (!QDir(m_pPath).exists())
    {
        if (!QDir().mkpath(m_pPath))
        {
            gtError() << "could not create path!" << " (" << m_pPath << ")";
            return false;
        }
    }

    QFile templateFile(QStringLiteral(":/templates/empty_project.xml"));

    if (!templateFile.open(QFile::ReadOnly | QFile::Text))
    {
        gtError() << "could not open template file!";
        return false;
    }

    // converting file content to string
    QTextStream in(&templateFile);
    QString data = in.readAll();

    QString str = QStringLiteral("<MODULE name=\"$$module$$\"/>");

    data.replace(QStringLiteral("$$projectname$$"), m_pName);

    // footprint
    GtFootprint footprint;

    data.replace(QStringLiteral("$$footprint$$"), footprint.exportToXML());

    QString modStr;

    // converting module meta data
    foreach (const QString& modStrItem, m_pModules)
    {
        QString tmpStr = str;
        modStr.append(tmpStr.replace(QStringLiteral("$$module$$"), modStrItem));
    }

    data.replace(QStringLiteral("$$modules$$"), modStr);

    templateFile.close();

    // write filled template to file
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        gtError() << "could not generate project file!";
        return false;
    }

    QTextStream out(&file);
    out << data;
    file.close();

    return true;
}

bool
GtProjectProvider::modifyMainProjectFile()
{
    if (!m_project)
    {
        return false;
    }

    const QString mainFilename = m_pPath + QDir::separator() +
                                 GtProject::mainFilename();

    QFile file(mainFilename);

    if (!file.exists())
    {
        gtError() << "project file does not exists in directory!";
        return false;
    }

    QDomDocument document;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!document.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
    {
        gtError() << tr("XML ERROR!") << " " << tr("line") << ": "
                  << errorLine << " " << tr("column") << ": "
                  << errorColumn << " -> " << errorStr;
        return false;
    }

    file.close();

    QDomElement root = document.documentElement();

    if (root.isNull() || (root.tagName() != QLatin1String("GTLAB")))
    {
        gtError() << "ERROR: Invalid GTlab project file!";
        return false;
    }

    QDomElement modElement = root.firstChildElement(QStringLiteral("MODULES"));

    if (modElement.isNull())
    {
        gtError() << "ERROR: Invalid GTlab project file!";
        return false;
    }

    QDomElement me = modElement.firstChildElement(QStringLiteral("MODULE"));

    while (!me.isNull())
    {
        QString id = me.attribute(QStringLiteral("name"));

        qDebug() << "checking module " << id << "...";

        if (!m_pModules.contains(id))
        {
            // remove entry
            qDebug() << "removing child " << id << "...";
            modElement.removeChild(me);
        }

        me = me.nextSiblingElement(QStringLiteral("MODULE"));
    }

    // write filled template to file
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        gtError() << tr("could not open module file!");
        return false;
    }

    QTextStream stream(&file);
    stream << document.toString(5);
    file.close();

    return true;
}

bool
GtProjectProvider::generateModuleFiles()
{
    if (m_pModules.isEmpty())
    {
        return true;
    }

    return std::all_of(std::begin(m_pModules), std::end(m_pModules), [this](const QString& modStrItem) {
        return generateModuleFile(modStrItem);
    });
}

bool
GtProjectProvider::generateModuleFile(const QString& id)
{
    if (m_pName.isEmpty() || m_pPath.isEmpty())
    {
        gtError() << "project name or project path is empty!";
        return false;
    }

    if (!QDir(m_pPath).exists())
    {
        if (!QDir().mkpath(m_pPath))
        {
            gtError() << "could not create path!" << " (" << m_pPath << ")";
            return false;
        }
    }

    QFile templateFile(QStringLiteral(":/templates/empty_module.xml"));

    if (!templateFile.open(QFile::ReadOnly | QFile::Text))
    {
        gtError() << "could not open template file!";
        return false;
    }

    // converting file content to string
    QTextStream in(&templateFile);
    QString data = in.readAll();

    data.replace(QStringLiteral("$$uuid$$"),
                 QUuid::createUuid().toString());

    const QString filename = moduleFilename(id.toLower());

    if (filename.isEmpty())
    {
        gtError() << tr("could not generate module file")
                  << QStringLiteral("! ") << tr("empty file name")
                  << QStringLiteral("!");
        return false;
    }

    QFile file(filename);

    if (file.exists())
    {
        gtError() << tr("could not generate module file")
                  << QStringLiteral("! ") << tr("file already exists")
                  << QStringLiteral("!");
        return false;
    }

    // write filled template to file
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        gtError() << "could not generate module file!";
        return false;
    }

    QTextStream out(&file);
    out << data;
    file.close();

    return true;
}

QList<GtObjectLinkProperty*>
GtProjectProvider::objectLinkProperties(GtObject* obj)
{
    // check object
    if (!obj)
    {
        return QList<GtObjectLinkProperty*>();
    }

    QList<GtObjectLinkProperty*> retval;

    foreach (GtAbstractProperty* prop, fullObjectPropertyList(obj))
    {
        GtObjectLinkProperty* linkProp =
                qobject_cast<GtObjectLinkProperty*>(prop);

        if (linkProp)
        {
            retval << linkProp;
        }
    }

    return retval;
}

QList<GtAbstractProperty*>
GtProjectProvider::fullObjectPropertyList(GtObject* obj)
{
    // check object
    if (!obj)
    {
        return QList<GtAbstractProperty*>();
    }

    QList<GtAbstractProperty*> retval;

    foreach (GtAbstractProperty* prop, obj->properties())
    {
        retval << fullPropertyList(prop);
    }

    return retval;
}

QList<GtAbstractProperty*>
GtProjectProvider::fullPropertyList(GtAbstractProperty* p)
{
    // check property
    if (!p)
    {
        return QList<GtAbstractProperty*>();
    }

    QList<GtAbstractProperty*> retval;

    retval << p;

    foreach (GtAbstractProperty* childProp, p->fullProperties())
    {
        retval.append(fullPropertyList(childProp));
    }

    return retval;
}

void
GtProjectProvider::resetUuid(GtObject* obj, QMap<QString, QString>& mapping)
{
    // check object
    if (!obj)
    {
        return;
    }

    QString oldUuid = obj->uuid();
    obj->newUuid();

    mapping.insert(oldUuid, obj->uuid());

//    foreach (GtObject* child, obj->findDirectChildren<GtObject*>())
//    {
//        resetUuid(child, mapping);
//    }
}

QString
GtProjectProvider::moduleFilename(const QString& id)
{
    if (m_pPath.isEmpty())
    {
        return QString();
    }

    if (id.isEmpty())
    {
        return QString();
    }

    return m_pPath + QDir::separator() + id.toLower() + "." +
           GtProject::moduleExtension();
}

