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
#include <QDateTime>
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
#include "gt_footprint.h"
#include "gt_versionnumber.h"
#include "gt_logging.h"
#include "gt_externalizedobject.h"
#include "gt_externalizationmanager.h"
#include "gt_moduleupgrader.h"
#include "gt_projectanalyzer.h"

GtProject::GtProject(const QString& path) :
    m_path(path),
    m_pathProp(QStringLiteral("path"), tr("Path"), tr("Project path"), path)
{
    m_valid = loadMetaData();
    m_upgradesAvailable = checkForUpgrades();

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

QString
GtProject::comment() const
{
    return m_comment;
}

void
GtProject::setComment(const QString& comment)
{
    m_comment = comment;
    changed();
}

void
GtProject::setInternalizeOnSave(bool value)
{
    m_internalizeOnSave = value;
}

QStringList
GtProject::availableModuleUpgrades() const
{
    GtProjectAnalyzer analyzer(this);
    GtFootprint footprint = analyzer.footPrint();

    return GtModuleUpgrader::instance().availableModuleUpgrades(
                footprint.modules());
}

QList<GtVersionNumber>
GtProject::availableUpgrades(const QString& moduleId)
{
    GtProjectAnalyzer analyzer(this);
    GtFootprint footprint = analyzer.footPrint();

    QMap<QString, GtVersionNumber> savedMods = footprint.modules();

    if (savedMods.contains(moduleId))
    {
        GtVersionNumber savedVer = savedMods.value(moduleId);

        return GtModuleUpgrader::instance().availableUpgrades(moduleId,
                                                             savedVer);
    }

    return {};
}

void
GtProject::upgradeProjectData()
{
    if (!isValid())
    {
        return;
    }

    const QStringList availUpgrades = availableModuleUpgrades();

    if (availUpgrades.isEmpty())
    {
        return;
    }

    GtProjectAnalyzer analyzer(this);
    GtFootprint footprint = analyzer.footPrint();

    QDir pdir(m_path);
    pdir.setNameFilters(QStringList() << QStringLiteral("*.gtmod"));

    QStringList entryList;

    for (auto const& modFiles : pdir.entryList(QDir::Files))
    {
        entryList << pdir.absoluteFilePath(modFiles);
    }

    entryList << pdir.absoluteFilePath(mainFilename());

    gtDebug() << "upgrading files: " << entryList;

    GtModuleUpgrader::instance().upgrade(footprint.modules(), entryList);

    // update project footprint for updated module
    updateModuleFootprint(availUpgrades);

    m_valid = loadMetaData();
    m_upgradesAvailable = checkForUpgrades();
}

void
GtProject::createBackup() const
{
    if (!isValid())
    {
        return;
    }

    QDir pdir(m_path);
    pdir.setNameFilters(QStringList() << QStringLiteral("*.gtmod"));

    QStringList entryList = pdir.entryList(QDir::Files);

    QString timeStamp = QDateTime::currentDateTime().toString(
                "yyyyMMddhhmmss");

    QDir bdir(m_path + QDir::separator() + "backup" + QDir::separator() +
              timeStamp);

    if (bdir.exists())
    {
        gtError() << "backup folder already exists!";
        return;
    }

    if (!bdir.mkpath("."))
    {
        gtError() << "could not create backup path!";
        return;
    }

    // backup module files
    foreach (const QString& entry, entryList)
    {
        QFile file(pdir.absoluteFilePath(entry));
        file.copy(bdir.absoluteFilePath(entry));
    }

    // backup project file
    QFile file(pdir.absoluteFilePath(mainFilename()));
    file.copy(bdir.absoluteFilePath(mainFilename()));
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

    // read comment
    QDomElement cdata = root.firstChildElement(QStringLiteral("comment"));

    if (!cdata.isNull())
    {
        //gtDebug() << "(" << projectname << ") comment found!";
        m_comment = cdata.text();
    }

    // module meta data
    readModuleMetaData(root);

//    qDebug() << "... done!";
    qDebug() << "project meta data loaded! (" << objectName() << ")";

    return true;
}

bool
GtProject::checkForUpgrades() const
{
    if (!m_valid)
    {
        return false;
    }

    GtProjectAnalyzer analyzer(this);
    GtFootprint footprint = analyzer.footPrint();

    return GtModuleUpgrader::instance().upgradesAvailable(footprint.modules());
}

void
GtProject::readModuleMetaData(const QDomElement& root)
{
    m_moduleIds.clear();

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
                        <<  tr("Multiple module definition!")
                        << " (" << mid << ")";
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
GtProject::readLabelData(const GtObjectList& moduleData)
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

        if (!obj)
        {
            gtWarning() << objectName() << ": "
                        << tr("Failed to create module package!")
                        << " (" << mid << ")";
            continue;
        }

        GtPackage* package = qobject_cast<GtPackage*>(obj);

        if (!package)
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

        /// all externalized objects should fetch its initial version
        for (auto* o : package->findChildren<GtExternalizedObject*>())
        {
            o->setFetchInitialVersion(true);
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

    // externalize or internalize objects accordingly
    if (!saveExternalizedObjectData())
    {
        gtError() << tr("Failed to save externalized object data!");
        // saving may continue
    }

    foreach (const QString& mid, m_moduleIds)
    {
        gtDebug().noquote() << tr("saving module data")
                           << QStringLiteral("\"") + mid +
                              QStringLiteral("\" ...");

        GtPackage* package = findPackage(mid);

        if (!package)
        {
            gtWarning().noquote()
                    << tr("Failed to save module data!")
                    << QStringLiteral("(\"") + mid + QStringLiteral("\")");
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
            gtWarning().noquote()
                    << tr("Failed to save module data!")
                    << QStringLiteral("(\"") + mid + QStringLiteral("\")");
            continue;
        }

        QString filename = m_path + QDir::separator() + mid.toLower() +
                           moduleExtension();

        if (!saveProjectFiles(filename, document))
        {
            gtWarning() << "\t |->" << mid;
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

    gtDebug().noquote() << tr("saving project")
                        << QStringLiteral("\"") + objectName() +
                           QStringLiteral("\" ...");

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

    // footprint
    QDomDocument footPrintDoc;
    GtFootprint footPrint;
    footPrintDoc.setContent(footPrint.exportToXML(), true);

    rootElement.appendChild(footPrintDoc.documentElement());

    QDomElement commentElement =
            document.createElement(QStringLiteral("comment"));
    QDomText cTxt = document.createTextNode(m_comment);
    commentElement.appendChild(cTxt);
    rootElement.appendChild(commentElement);

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

    return saveProjectFiles(filename, document);
}

bool
GtProject::saveExternalizedObjectData()
{
    bool success{true};
    auto objects = findChildren<GtExternalizedObject*>();

    // force internalization of all objects
    if (m_internalizeOnSave)
    {
        gtDebug() << "internalizing object data...";

        // externalization must be enabled
        auto value = gtExternalizationManager->isExternalizationEnabled();
        gtExternalizationManager->enableExternalization(true);

        int counter = 0;
        for (auto* obj : qAsConst(objects))
        {
            success &= obj->internalize();
            counter += obj->isFetched();
        }

        gtExternalizationManager->enableExternalization(value);

        gtInfo() << tr("successfully internalized") << counter
                 << tr("out of") << objects.count() << tr("objects!");

        return success;
    }

    if (!gtExternalizationManager->isExternalizationEnabled())
    {
        return true;
    }

    gtDebug() << "saving externalized object data...";

    for (auto* obj : qAsConst(objects))
    {
        // only externalize the object if its not referenced as the data wont be
        // cleared otherwise and will be saved as memento
        if (obj->refCount() == 0)
        {
            success &= obj->externalize();
        }
    }

    return success;
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
                                   gtApp->moduleVersion(mid).toString());
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

    if (pd)
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

    if (ld)
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

        if (l)
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

void
GtProject::updateModuleFootprint(const QStringList& modIds)
{
    gtDebug() << "GtProject::updateModuleFootprint...";

    QString filename = m_path + QDir::separator() + mainFilename();

    QFile file(filename);

    if (!file.exists())
    {
        qWarning() << "WARNING: file does not exists!";
        qWarning() << " |-> " << filename;
        return;
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
        return;
    }

    QDomElement pdata = document.documentElement();

    if (pdata.isNull() || (pdata.tagName() != QLatin1String("GTLAB")))
    {
        gtDebug() << "ERROR: Invalid GTlab project file!";
        return;
    }

    QDomElement footprint = pdata.firstChildElement(
                QStringLiteral("env-footprint"));


    if (footprint.isNull())
    {
        return;
    }

    QDomElement mods = footprint.firstChildElement(QStringLiteral("modules"));

    if (mods.isNull())
    {
        return;
    }

    QDomElement mod = mods.firstChildElement(QStringLiteral("module"));
    while (!mod.isNull())
    {
        QDomElement modIdEl = mod.firstChildElement(QStringLiteral("id"));
        QDomElement modVerEl = mod.firstChildElement(QStringLiteral("ver"));

        if (!modIdEl.isNull() && !modVerEl.isNull())
        {
            if (modIds.contains(modIdEl.text()))
            {
                gtDebug() << "updating footprint " << modIdEl.text() << "...";
                gtDebug() << "file " << filename << "...";
                gtDebug() << "  |-> old = " << modVerEl.text();
                gtDebug() << "  |-> new = "
                          << gtApp->moduleVersion(modIdEl.text()).toString();

                modVerEl.firstChild().setNodeValue(
                            gtApp->moduleVersion(modIdEl.text()).toString());
            }
        }

        mod = mod.nextSiblingElement(QStringLiteral("module"));
    }

    if (!GtXmlUtilities::writeDomDocumentToFile(filename,
                                                document,
                                                true))
    {
        gtError() << filename << QStringLiteral(": ")
                  << "Failed to save project data!";
    }
}

GtProject::~GtProject()
{
//    qDebug() << "project deleted!";
}

const QString&
GtProject::path() const
{
    return m_path;
}

QString
GtProject::moduleDataPath(const QString &moduleId) const
{
    return path() + QDir::separator() + moduleId.toLower() +
            GtProject::moduleExtension();
}

bool
GtProject::isValid() const
{
    return m_valid;
}

bool
GtProject::isOpen() const
{
    // TODO: logic for open or closed projects

    return (childCount<GtObject*>() != 0);
}

bool
GtProject::upgradesAvailable() const
{
    return m_upgradesAvailable;
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
GtProject::findPackage(const QString& mid)
{
    // class name of package
    QString pkgId{ gtApp->modulePackageId(mid) };

    auto childs = findDirectChildren<GtPackage*>();
    auto iter = std::find_if(std::begin(childs), std::end(childs),
                 [&pkgId](const GtPackage* pkg) {
        return pkg->metaObject()->className() == pkgId;
    });

    return iter != std::end(childs) ? *iter : nullptr;
}

const QStringList&
GtProject::moduleIds()
{
    return m_moduleIds;
}

int
GtProject::numberOfLabelUsages(GtLabel* label)
{
    if (!label)
    {
        return -1;
    }

    return findLabelUsages(label).size();
}

QList<GtLabel*>
GtProject::findLabelUsages(GtLabel* label)
{
    QList<GtLabel*> usages;

    if (!label)
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

    if (!group)
    {
        return false;
    }

    QList<GtPackage*> packages = group->findDirectChildren<GtPackage*>();

    foreach (GtPackage* package, packages)
    {
        GtPackage* oldPkg = findDirectChild<GtPackage*>(package->objectName());

        if (!oldPkg)
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

    if (!gtApp->session())
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

QString
GtProject::readFootprint() const
{
    QString filename = m_path + QDir::separator() + mainFilename();

    QFile file(filename);

    if (!file.exists())
    {
        qWarning() << "WARNING: file does not exists!";
        qWarning() << " |-> " << filename;

        return QString();
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

        return QString();
    }

    QDomElement root = document.documentElement();

    if (root.isNull() || (root.tagName() != QLatin1String("GTLAB")))
    {
        gtDebug() << "ERROR: Invalid GTlab project file!";
        return QString();
    }

    QString retval;

    QDomElement ftprnt =
            root.firstChildElement(QStringLiteral("env-footprint"));

    if (ftprnt.isNull())
    {
        gtDebug() << "Footprint not found in project file!";
        return QString();
    }

    QTextStream stream(&retval);
    ftprnt.save(stream, 4);

    return retval;
}
