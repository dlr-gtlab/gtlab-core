/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDomDocument>
#include <QXmlStreamWriter>
#include <QDir>
#include <QDebug>

#include "gt_project.h"
#include "gt_processdata.h"
#include "gt_task.h"
#include "gt_processfactory.h"
#include "gt_factorygroup.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_coreapplication.h"
#include "gt_package.h"
#include "gt_labeldata.h"
#include "gt_label.h"
#include "gt_loadprojecthelper.h"
#include "gt_xmlutilities.h"
#include "gt_logging.h"

GtProject::GtProject(const QString& path) :
    m_path(path),
    m_pathProp(QStringLiteral("path"), tr("Path"), tr("Project path"), path)
{
    m_valid = loadMetaData();

    m_pathProp.setReadOnly(true);

    registerProperty(m_pathProp);
}

void
GtProject::setModuleIds(const QStringList& list)
{
    if (isOpen())
    {
        // TODO: set ne module ids for opened project
    }
    else
    {
        m_moduleIds = list;
    }
}

const QString
GtProject::mainFilename()
{
    return QStringLiteral("project") + mainFileExtension();
}

const QString
GtProject::mainFileExtension()
{
    return QStringLiteral(".gtlab");
}

const QString
GtProject::moduleExtension()
{
    return QStringLiteral(".gtmod");
}

bool
GtProject::loadMetaData()
{
//    qDebug() << "loading " << m_path << "...";

    QDomElement root = readProjectData();

    if (root.isNull())
    {
        qWarning() << "WARNING: root is null!";
        return false;
    }

    QString projectname = root.attribute(QStringLiteral("projectname"));
    if (projectname.isEmpty())
    {
        qDebug() << "ERROR: Invalid GTlab project file!";
        return false;
    }

    setObjectName(projectname);

    // module meta data
    readModuleMetaData(root);

//    qDebug() << "... done!";
    qDebug() << "project meta data loaded! (" << objectName() << ")";

    return true;
}

void
GtProject::readModuleMetaData(QDomElement& root)
{
    /* module informations */
    QDomElement mdata = root.firstChildElement(QStringLiteral("MODULES"));

    if (mdata.isNull())
    {
        return;
    }

    QDomElement module = mdata.firstChildElement(QStringLiteral("MODULE"));
    while (!module.isNull())
    {
        QString mid = module.attribute(QStringLiteral("name"));

        if (mid.isEmpty())
        {
            gtWarning() << objectName() << ": " <<  tr("Invalid module found!");
        }
        else if (m_moduleIds.contains(mid))
        {
            gtWarning() << objectName() << ": "
                        <<  tr("Multiple module definition!");
        }
        else
        {
            if (!gtApp->moduleIds().contains(mid))
            {
                gtWarning() << objectName() << ": " << tr("Unknown module id!")
                            << " (" << mid << ")";
            }
            m_moduleIds << mid;
        }

        module = module.nextSiblingElement(QStringLiteral("MODULE"));
    }


}

GtObject*
GtProject::readProcessData()
{
    GtProcessData* data = new GtProcessData;
    data->setFactory(gtObjectFactory);
    data->setDefault(true);

    // read process data here

    QDomElement root = readProjectData();

    if (!root.isNull())
    {
        /* process informations */
        QDomElement pdata = root.firstChildElement(QStringLiteral("PROCESSES"));

        if (!pdata.isNull())
        {
            QDomElement pe = pdata.firstChildElement(QStringLiteral("object"));
            while (!pe.isNull())
            {
//                QString fieldClass = pe.attribute(QStringLiteral("class"));

//                if (fieldClass ==
//                        QLatin1String(GtTask::staticMetaObject.className()))
//                {
                    GtObjectMemento memento(pe);
                    if (!memento.isNull())
                    {
                        GtObject* obj = memento.restore(gtProcessFactory);
                        if (obj)
                        {
                            GtTask* p = qobject_cast<GtTask*>(obj);
                            if (p)
                            {
                                data->appendChild(p);
                            }
                            else
                            {
                                gtWarning() << tr("could not recreate object")
                                            << " (" << tr("casting failed")
                                            << ")";
                                delete obj;
                            }
                        }
                    }
//                }

                pe = pe.nextSiblingElement(QStringLiteral("object"));
            }
        }
    }

    return data;
}

GtObject*
GtProject::readLabelData(GtObjectList& moduleData)
{
    GtLabelData* data = new GtLabelData;
    data->setDefault(true);
    data->setFactory(gtObjectFactory);

    // read saved label data here
    QDomElement root = readProjectData();

    if (!root.isNull())
    {
        /* label informations */
        QDomElement ldata = root.firstChildElement(QStringLiteral("LABELS"));

        if (!ldata.isNull())
        {
            QDomElement le = ldata.firstChildElement(QStringLiteral("object"));
            while (!le.isNull())
            {
                QString fieldClass = le.attribute(QStringLiteral("class"));

                if (fieldClass ==
                        QLatin1String(GtLabel::staticMetaObject.className()))
                {
                    GtObjectMemento memento(le);
                    if (!memento.isNull())
                    {
                        GtObject* obj = memento.restore(gtObjectFactory);
                        if (obj)
                        {
                            GtLabel* l = qobject_cast<GtLabel*>(obj);
                            if (l)
                            {
                                if (!data->addLabel(l))
                                {
                                    gtWarning() << tr("could not add label")
                                                << " (" << tr("already exists")
                                                << ")";
                                    delete obj;
                                }
                            }
                            else
                            {
                                gtWarning() << tr("could not recreate label")
                                            << " (" << tr("casting failed")
                                            << ")";
                                delete obj;
                            }
                        }
                    }
                }

                le = le.nextSiblingElement(QStringLiteral("object"));
            }
        }
    }

    // read datamodel label entries
    foreach (GtObject* obj, moduleData)
    {
        QList<GtLabel*> labels = obj->findChildren<GtLabel*>();
        foreach (GtLabel* label, labels)
        {
            data->addLabel(label->objectName());
        }
    }

    // add default label
    data->addDefaultLabel();

    return data;
}

GtObjectList
GtProject::readModuleData()
{
    GtObjectList retval;

    foreach (const QString& mid, m_moduleIds)
    {
        if (!gtApp->moduleIds().contains(mid))
        {
            continue;
        }

        QString packageId = gtApp->modulePackageId(mid);
        if (packageId.isEmpty())
        {
            gtWarning() << objectName() << ": "
                        << tr("Module has no datamodel interface!")
                        << " (" << mid << ")";
            continue;
        }

        // check for old module file and rename it
        renameOldModuleFile(m_path, mid);

        QString filename = m_path + QDir::separator() + mid.toLower() +
                           moduleExtension();

        QFile file(filename);

        if (!file.exists())
        {
            gtWarning() << objectName() << ": "
                      << tr("GTlab module file not found!") << "(" << mid
                      << ")";
            continue;
        }

        QDomDocument document;

        QString errorStr;
        int errorLine;
        int errorColumn;

        if (!document.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
        {
            gtWarning() << tr("XML ERROR!") << " " << tr("line") << ": "
                      << errorLine << " " << tr("column") << ": "
                      << errorColumn << " -> " << errorStr;
            continue;
        }

        QDomElement root = document.documentElement();

        if (root.isNull() || (root.tagName() != QLatin1String("GTLABMODULE")))
        {
            gtWarning() << tr("Invalid GTlab module file!");
            continue;
        }

        const QString uuid = root.attribute(QStringLiteral("uuid"));

        if (uuid.isEmpty())
        {
            gtWarning() << tr("Invalid GTlab module file!");
            continue;
        }

        GtObject* obj = gtObjectFactory->newObject(packageId);

        if (obj == nullptr)
        {
            gtWarning() << objectName() << ": "
                        << tr("Failed to create module package!")
                        << " (" << mid << ")";
            continue;
        }

        GtPackage* package = qobject_cast<GtPackage*>(obj);

        if (package == nullptr)
        {
            gtWarning() << objectName() << ": "
                        << tr("Failed to create module package!")
                        << " (" << mid << ")";
            continue;
        }

        package->setUuid(uuid);

        if (!package->readData(root))
        {
            gtWarning() << objectName() << ": "
                        << tr("Failed to read module data!")
                        << " (" << mid << ")";
            continue;
        }

        retval.append(obj);
    }

    return retval;
}

bool
GtProject::saveModuleData()
{
    if (!isOpen())
    {
        return false;
    }

//    gtDebug() << "saving " << objectName() << "...";

    foreach (const QString& mid, m_moduleIds)
    {
        gtDebug() << "saving " << mid << "...";

        GtPackage* package = findPackage(mid);

        if (package == Q_NULLPTR)
        {
            gtWarning() << objectName() << QStringLiteral(": ")
                        << tr("Failed to save module data!")
                        << QStringLiteral(" (") << mid << QStringLiteral(")");
            continue;
        }

        QDomDocument document;
        QDomProcessingInstruction header = document.createProcessingInstruction(
                QStringLiteral("xml"),
                QStringLiteral("version=\"1.0\" encoding=\"utf-8\""));
        document.appendChild(header);

        QDomElement rootElement =
                document.createElement(QStringLiteral("GTLABMODULE"));

        rootElement.setAttribute(QStringLiteral("uuid"), package->uuid());

        document.appendChild(rootElement);

        if (!package->saveData(rootElement, document))
        {
            gtWarning() << objectName() << QStringLiteral(": ")
                        << tr("Failed to save module data!")
                        << QStringLiteral(" (") << mid << QStringLiteral(")");
            continue;
        }

        QString filename = m_path + QDir::separator() + mid.toLower() +
                           moduleExtension();

        if (!saveProjectFiles(filename, document))
        {
            gtWarning() << "\t |->" << QStringLiteral(" (") << mid
                        << QStringLiteral(")");

            continue;
        }
    }

    return true;
}

bool
GtProject::saveProjectOverallData()
{
    if (!isOpen())
    {
        return false;
    }

    QDomDocument document;
    QDomProcessingInstruction header = document.createProcessingInstruction(
            QStringLiteral("xml"),
            QStringLiteral("version=\"1.0\" encoding=\"utf-8\""));
    document.appendChild(header);

    QDomElement rootElement =
            document.createElement(QStringLiteral("GTLAB"));

    rootElement.setAttribute(QStringLiteral("projectname"), objectName());
    rootElement.setAttribute(QStringLiteral("version"),
                             gtApp->versionToString());

    if (!saveModuleMetaData(rootElement, document))
    {
        return false;
    }

    if (!saveProcessData(rootElement, document))
    {
        return false;
    }

    if (!saveLabelData(rootElement, document))
    {
        return false;
    }

    document.appendChild(rootElement);

    QString filename = m_path + QDir::separator() + mainFilename();

    if (!saveProjectFiles(filename, document))
    {
        return false;
    }

    return true;
}

bool
GtProject::saveModuleMetaData(QDomElement& root, QDomDocument& doc)
{
    QDomElement modulesElement = doc.createElement(QStringLiteral("MODULES"));

    foreach (const QString& mid, m_moduleIds)
    {
        QDomElement moduleElement = doc.createElement(QStringLiteral("MODULE"));
        moduleElement.setAttribute(QStringLiteral("name"), mid);
        moduleElement.setAttribute(QStringLiteral("version"),
                                   gtApp->moduleVersion(mid));
        modulesElement.appendChild(moduleElement);
    }

    root.appendChild(modulesElement);

    return true;
}

bool
GtProject::saveProcessData(QDomElement& root, QDomDocument& doc)
{
    QDomElement pdElement = doc.createElement(QStringLiteral("PROCESSES"));

    GtProcessData* pd = processData();

    if (pd != Q_NULLPTR)
    {
        foreach (GtTask* task, pd->findDirectChildren<GtTask*>())
        {
            GtObjectMemento memento = task->toMemento();
            pdElement.appendChild(memento.documentElement());
        }
    }

    root.appendChild(pdElement);

    return true;
}

bool
GtProject::saveLabelData(QDomElement& root, QDomDocument& doc)
{
    QDomElement ldElement = doc.createElement(QStringLiteral("LABELS"));

    GtLabelData* ld = labelData();

    if (ld != Q_NULLPTR)
    {
        foreach (GtLabel* label, ld->findDirectChildren<GtLabel*>())
        {
            GtObjectMemento memento = label->toMemento();
            ldElement.appendChild(memento.documentElement());
        }
    }

    root.appendChild(ldElement);

    return true;
}

QDomElement
GtProject::readProjectData()
{
    QString filename = m_path + QDir::separator() + mainFilename();

    QFile file(filename);

    if (!file.exists())
    {
        qWarning() << "WARNING: file does not exists!";
        qWarning() << " |-> " << filename;
        return QDomElement();
    }

    QDomDocument document;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!document.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
    {
        gtDebug() << tr("XML ERROR!") << " " << tr("line") << ": "
                  << errorLine << " " << tr("column") << ": "
                  << errorColumn << " -> " << errorStr;
        return QDomElement();
    }

//    if(!GtdUtilities::fileContentToDomDocument(file, document))
//    {
//        qWarning() << "WARNING: could not transfer file content to document!";
//        return QDomElement();
//    }

    QDomElement root = document.documentElement();

    if (root.isNull() || (root.tagName() != QLatin1String("GTLAB")))
    {
        gtDebug() << "ERROR: Invalid GTlab project file!";
        return QDomElement();
    }

    return root;
}

QList<GtLabel*>
GtProject::findLabelUsagesHelper(GtObject* obj, GtLabel* label)
{
    QList<GtLabel*> retval;

    QList<GtObject*> childs = obj->findDirectChildren<GtObject*>();

    foreach (GtObject* c, childs)
    {
        GtLabel* l = qobject_cast<GtLabel*>(c);

        if (l != Q_NULLPTR)
        {
            if (l->objectName() == label->objectName())
            {
                retval.append(l);
            }
        }
        else
        {
            retval.append(findLabelUsagesHelper(c, label));
        }
    }

    return retval;
}

void
GtProject::renameOldModuleFile(const QString& path, const QString& modId)
{
    QString filename = path + QDir::separator() + modId.toLower() +
                       mainFileExtension();

    QFile file(filename);

    if (!file.exists())
    {
        // no old module files found - everything ok
        return;
    }

    file.rename(path + QDir::separator() + modId.toLower() + moduleExtension());
}

bool
GtProject::saveProjectFiles(const QString& filePath, const QDomDocument& doc)
{
    /// create file with name 'path + _new'
    const QString tempFilePath = filePath + QStringLiteral("_new");

    // new ordered attribute stream writer algorithm
    if (!GtXmlUtilities::writeDomDocumentToFile( tempFilePath, doc, true))
    {
        gtError() << objectName() << QStringLiteral(": ")
                  << tr("Failed to save project data!");

        return false;
    }

    //rename files
    /// => existing from 'path' to 'path + _backup'
    /// => the new file from 'path + _new' to 'path'

    /// rename existing file (old state)
    QFile origFile(filePath);

    if (origFile.exists())
    {
        /// remove old backup file
        QFile backupFile(filePath + QStringLiteral("_backup"));

        if (backupFile.exists())
        {
            if (!backupFile.remove())
            {
                gtError() << "Could not delete existing backup file ' "
                          << backupFile.fileName() << "!";

                return false;
            }
        }

        /// rename active file to backup
        if (!origFile.rename(filePath + QStringLiteral("_backup")))
        {
            gtError() << "Could not rename '" << origFile.fileName()
                      << "' to '" << filePath + QStringLiteral("_backup")
                      << "'!";

            return false;
        }
    }

    /// rename new file to active (new state)
    if (!QFile(tempFilePath).rename(filePath))
    {
        gtError() << "Could not rename project file ('" << tempFilePath
                  << "' to '" << filePath << "'!";

        return false;
    }

    return true;
}

GtProject::~GtProject()
{
//    qDebug() << "project deleted!";
}

const QString&
GtProject::path()
{
    return m_path;
}

bool
GtProject::isValid()
{
    return m_valid;
}

bool
GtProject::isOpen()
{
    // TODO: logic for open or closed projects

    if (childCount<GtObject*>() == 0)
    {
        return false;
    }

    return true;
}

GtProcessData*
GtProject::processData()
{
    return findDirectChild<GtProcessData*>(QStringLiteral("Process Data"));
}

GtLabelData*
GtProject::labelData()
{
    return findDirectChild<GtLabelData*>(QStringLiteral("Label Data"));
}

GtTask*
GtProject::findProcess(const QString& val)
{
    GtTask* retval = nullptr;

    GtProcessData* pdata = processData();

    if (pdata)
    {
        return pdata->findProcess(val);
    }

    return retval;
}

GtPackage*
GtProject::findPackage(const QString& val)
{
    return findDirectChild<GtPackage*>(val);
}

const QStringList&
GtProject::moduleIds()
{
    return m_moduleIds;
}

int
GtProject::numberOfLabelUsages(GtLabel* label)
{
    if (label == Q_NULLPTR)
    {
        return -1;
    }

    return findLabelUsages(label).size();
}

QList<GtLabel*>
GtProject::findLabelUsages(GtLabel* label)
{
    QList<GtLabel*> usages;

    if (label == Q_NULLPTR)
    {
        return usages;
    }

    QList<GtPackage*> packages = findDirectChildren<GtPackage*>();

    foreach (GtPackage* package, packages)
    {
        usages.append(findLabelUsagesHelper(package, label));
    }

    return usages;
}

GtObjectMemento
GtProject::toProjectDataMemento()
{
    qDebug() << "GtProject::toProjectDataMemento()";
    GtObjectGroup* group = new GtObjectGroup;
    group->setUuid(QStringLiteral("-"));

    QList<GtPackage*> packages = findDirectChildren<GtPackage*>();

    foreach (GtPackage* package, packages)
    {
        GtObject* obj = package->clone();

        if (obj)
        {
            group->appendChild(obj);
        }
    }

    GtObjectMemento memento = group->toMemento();

    delete group;

    return memento;
}

bool
GtProject::fromProjectDataMemento(GtObjectMemento& memento)
{
    if (memento.isNull())
    {
        return false;
    }

    GtObjectGroup* group = memento.restore<GtObjectGroup*>(gtObjectFactory);

    if (group == Q_NULLPTR)
    {
        return false;
    }

    QList<GtPackage*> packages = group->findDirectChildren<GtPackage*>();

    foreach (GtPackage* package, packages)
    {
        GtPackage* oldPkg = findDirectChild<GtPackage*>(package->objectName());

        if (oldPkg == Q_NULLPTR)
        {
            delete group;
            return false;
        }

        oldPkg->fromMemento(package->toMemento());
    }

    delete group;

    return true;
}

bool
GtProject::renameProject(const QString& str)
{
    if (isOpen())
    {
        return false;
    }

    if (str.isEmpty())
    {
        return false;
    }

    if (str == objectName())
    {
        return false;
    }

    if (gtApp->session() == Q_NULLPTR)
    {
        return false;
    }

    if (gtApp->session()->projectIds().contains(str))
    {
        return false;
    }

    // rename project within project file
    QDomElement projectData = readProjectData();

    if (projectData.isNull())
    {
        return false;
    }

    projectData.setAttribute(QStringLiteral("projectname"), str);

    QDomDocument document;
    QDomProcessingInstruction header = document.createProcessingInstruction(
            QStringLiteral("xml"),
            QStringLiteral("version=\"1.0\" encoding=\"utf-8\""));
    document.appendChild(header);

    document.appendChild(projectData);

    QString filename = m_path + QDir::separator() + mainFilename();

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gtWarning() << objectName() << QStringLiteral(": ")
                    << tr("Failed to save project data!");
        return false;
    }

    QTextStream stream(&file);
    stream << document.toString(5);

    file.close();

    setObjectName(str);

    acceptChanges();

    return true;
}
