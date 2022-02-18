/* GTlab - Gas Turbine laboratory
 * Source File: gt_h5filemanager.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 18.08.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_h5filemanager.h"

#include "gt_externalizationsettings.h"
#include "gt_externalizedh5object.h"
#include "gt_logging.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QThread>

const QString GtH5FileManager::S_H5_PROJECT_FILENAME =
        QStringLiteral("project.h5");
const QString GtH5FileManager::S_TEMP_DIRNAME =
        QStringLiteral("temp");

bool
GtH5FileManager::FileReference::isPathValid() const
{
    // check if file dir exists
    return QDir(QFileInfo(filePath).path()).exists();
}

GtH5FileManager::GtH5FileManager() :
    m_idCounter(0),
    m_projectObject(nullptr),
    m_processThread(nullptr)
{

}

GtH5FileManager*
GtH5FileManager::instance()
{
    static GtH5FileManager* self = new GtH5FileManager();

    return self;
}

GtH5FileManager::FileReference
GtH5FileManager::projectFileReference() const
{
    QString projectPath(m_projectPath);
    projectPath += QDir::separator() + S_H5_PROJECT_FILENAME;

    return FileReference{ projectPath, -1 };
}

int
GtH5FileManager::createNewTempFile()
{
    m_processThread = QThread::currentThread();
    m_idCounter += 1;
    m_fileMap.insert(m_idCounter, FileIndex{ generateTempFilePath(),
                                                     QMap<QString, int>() });

    return m_idCounter;
}

bool
GtH5FileManager::removeTempFile(int fileId)
{
    m_processThread = nullptr;
    if (!m_fileMap.contains(fileId))
    {
        return true;
    }

    QString filePath = m_fileMap.value(fileId).filePath;
    QFile file(filePath);

    // remove temp file
    if (filePath.isEmpty() || (file.exists() && !file.remove(filePath)))
    {
        gtError() << "removing temp file at '" + filePath +
                     "' failed! (probably locked?)";
        return false;
    }

    return m_fileMap.remove(fileId) > 0;
}

bool
GtH5FileManager::commitObjectsInTempFile(int fileId, const GtObjectList& modules)
{
    // externalize all objects that changed
    for (GtObject* obj : modules)
    {
        for (auto* externObj : obj->findChildren<GtExternalizedObject*>())
        {
            // externalize every object that is still fetched
            if (externObj->isFetched() && externObj->refCount() > 0)
            {
                gtDebug() << "externalizing:" << externObj->objectPath();
                externObj->setIsTemporary(true);
                externObj->releaseData(GtExternalizedObject::Externalize);
            }
        }
    }

    m_processThread = nullptr;

    // no objects where externalized
    if (!m_fileMap.contains(fileId))
    {
        gtDebug() << "no externalized objects to commit!";
        return false;
    }

    // retrieve the file struct of the file
    FileIndex file(m_fileMap.value(fileId));

    // acces project
    GtObject* project(m_projectObject);

    if (project == Q_NULLPTR)
    {
        gtError() << "commiting hdf5 file failed! (null project)";
        return false;
    }

    // iterate over each object within the hdf5 file
    for (const QString& uuid : file.uuidMap.keys())
    {
        // search for object by uuid in all temp module objects
        GtExternalizedH5Object* cloned = qobject_cast<GtExternalizedH5Object*>(
                    this->getObjectByUuidHelper(uuid, modules));
        // get object by uuid from the main project tree
        GtExternalizedH5Object* target = qobject_cast<GtExternalizedH5Object*>(
                    project->getObjectByUuid(uuid));

        commitObject(cloned, target);
    }

    return true;
}

bool
GtH5FileManager::commitObject(GtExternalizedH5Object* cloned,
                              GtExternalizedH5Object* target)
{
    if (cloned == Q_NULLPTR)
    {
        gtError() << "committing object failed! (null cloned object)";
        return false;
    }
    // target may not exist -> object is new
    if (target == Q_NULLPTR)
    {
        // commit dataset to main project tree
        if (!cloned->fetchData())
        {
            gtError() << "committing object failed! (fetching failed)";
            return false;
        }
        cloned->setIsTemporary(false);
        if (!cloned->releaseData(GtExternalizedObject::ForceExternalize))
        {
            gtError() << "committing object failed! (externalization failed)";
            return false;
        }
        return true;
    }

    // check if swapping is required
    if (cloned == target || !cloned->hasChanges(target->cachedHash()))
    {
        return true;
    }

    // fetch the data
    if (!cloned->fetchData() || !target->fetchData())
    {
        gtError() << "swapping datasets failed! (fetching failed)";
        return false;
    }

    // setup to externalize correctly
    // cloned object should wirte to the main project file
    cloned->setIsTemporary(false);
    // the target object should write to the temp file
    target->setIsTemporary(true);

    // swap the datasets
    bool success = cloned->releaseData(GtExternalizedObject::ForceExternalize)&&
                   target->releaseData(GtExternalizedObject::ForceExternalize);

    // reset
    target->setIsTemporary(false);
    cloned->setIsTemporary(true);

    if (!success)
    {
        gtError() << "swapping datasets failed! (externalization failed)";
    }
    return success;
}

GtH5FileManager::FileReference
GtH5FileManager::createFileReference(GtExternalizedH5Object* obj)
{
    if (!obj || (!obj->isTemporary() && obj->thread() != m_processThread) ||
        !m_fileMap.contains(m_idCounter))
    {
        return projectFileReference();
    }    

    obj->setIsTemporary(true);

    FileIndex fileIndex = m_fileMap.value(m_idCounter);

    fileIndex.uuidMap.insert(obj->uuid(), -1);

    m_fileMap.insert(m_idCounter, fileIndex);

    return FileReference{ fileIndex.filePath, -1 };
}

GtH5FileManager::FileReference
GtH5FileManager::getFileReference(const GtExternalizedH5Object* obj,
                                  int fileId) const
{
    // bounds checking
    fileId = (fileId < 1) ? m_idCounter:fileId;

    if (!obj || (!obj->isTemporary() && obj->thread() != m_processThread) ||
        !m_fileMap.contains(m_idCounter))
    {
        return projectFileReference();
    }

    FileIndex fileIndex = m_fileMap.value(fileId);

    if (!fileIndex.uuidMap.contains(obj->uuid()))
    {
        return projectFileReference();
    }

    return FileReference{ fileIndex.filePath,
                          fileIndex.uuidMap.value(obj->uuid()) };
}

bool
GtH5FileManager::updateFileReference(const GtExternalizedH5Object* obj,
                                     int h5Reference, int fileId)
{
    // bounds checking
    fileId = (fileId < 1) ? m_idCounter:fileId;

    if (!obj || (!obj->isTemporary() && obj->thread() != m_processThread) ||
        !m_fileMap.contains(m_idCounter))
    {
        return false;
    }

    FileIndex fileIndex = m_fileMap.value(fileId);

    if (!fileIndex.uuidMap.contains(obj->uuid()))
    {
        return false;
    }

    // update fileIndex
    fileIndex.uuidMap.insert(obj->uuid(), h5Reference);

    m_fileMap.insert(fileId, fileIndex);
    return true;
}


void
GtH5FileManager::reset(GtObject* project, const QString& projectDir)
{
    m_projectObject = project;
    m_projectPath = projectDir.toUtf8();
    m_fileMap.clear();
    m_idCounter = 0;

    QDir dir(projectDir);

    // create dir if it does not exist
    if (!dir.exists(S_TEMP_DIRNAME) && !dir.mkdir(S_TEMP_DIRNAME))
    {
        gtError() << "resetting h5 file manager failed (cannot create temp dir!)";
        return;
    }
    if (!dir.cd(S_TEMP_DIRNAME))
    {
        gtError() << "resetting h5 file manager failed (cannot cd to temp dir!)";
        return;
    }

    // remove all files in tmp dir
    QDirIterator iterator(dir.absolutePath(),
                          QStringList() << QStringLiteral("*.h5"),
                          QDir::Files);

    while (iterator.hasNext())
    {
        QString fileName = QFileInfo(iterator.next()).fileName();

        if (!fileName.endsWith(QStringLiteral(".h5"))) continue;
        if (!dir.remove(fileName))
        {
            gtError() << "removing temp file at '" + fileName +
                         "' failed! (probably locked?)";
        }
    }
}


QString
GtH5FileManager::generateTempFilePath() const
{
    return QDir::fromNativeSeparators(
                QString(m_projectPath) +
                QDir::separator() + S_TEMP_DIRNAME + QDir::separator() +
                QStringLiteral("temp_") + QString::number(m_idCounter) +
                QStringLiteral(".h5"));
}

GtObject*
GtH5FileManager::getObjectByUuidHelper(const QString& uuid,
                                       const QList<GtObject*>& modules)
{
    GtObject* retVal = Q_NULLPTR;

    for (GtObject* obj : modules)
    {
        if (obj == Q_NULLPTR)
        {
            continue;
        }

        // search for hdf5 object by uuid in module object
        retVal = obj->getObjectByUuid(uuid);

        // object found
        if (retVal != Q_NULLPTR)
        {
            return retVal;
        }
    }

    return retVal;
}

