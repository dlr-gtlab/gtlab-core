/* GTlab - Gas Turbine laboratory
 * Source File: gt_externalizedh5object.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 04.08.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_externalizedh5object.h"

#include "gt_externalizationsettings.h"
#include "gt_h5filemanager.h"
#include "gt_logging.h"

#if GT_H5
#include "gt_h5.h"
#endif

#include <QDir>

GtExternalizedH5Object::GtExternalizedH5Object() :
    m_reference(0)
{

}

bool
GtExternalizedH5Object::getDataSet(GtH5File& file, GtH5DataSet& dataset)
{
#if GT_H5
    GtH5FileManager::FileReference fileReference =
            gtH5FileManager->getFileReference(this);

    if (!fileReference.isPathValid())
    {
        gtError() << "hdf5 file cannot be accessed!"
                  << fileReference.filePath;
        return false;
    }

    // filePath must be unix styled
    QString filePath = QDir::fromNativeSeparators(fileReference.filePath);

    // check if file exists
    if (!GtH5File::fileExists(filePath) || !GtH5File::isValidH5File(filePath))
    {
        gtError() << "hdf5 file does not exist!"
                  << filePath;
        return false;
    }

    // open file
    file = GtH5File(filePath,  GtH5File::OpenReadOnly);

    if (!file.isValid())
    {
        return false;
    }

    // access dataset reference
    GtH5Reference ref(isTemporary() ? fileReference.h5Reference:getH5Reference(),
                      GtH5Reference::ObjectType::DataSet);

    // try retrieving by reference
    if (ref.isValid())
    {
        dataset = ref.toDataSet(file);

        if (dataset.isValid())
        {
            return true;
        }
        gtDebug() << "accessing dataset per refererence failed!";
    }

    // try retrieving by path
    QByteArrayList path = { metaObject()->className(), uuid().toUtf8() };

    // check if the path inside the hdf5 file exists
    if (file.root().exists(path))
    {
        dataset = file.root().openDataset(path.join("/"));
    }

    return dataset.isValid();
#else
    return false;
#endif
}

bool
GtExternalizedH5Object::createDataSet(GtH5File& file,
                                      GtH5DataSet& dataset,
                                      const GtH5DataType& dataType,
                                      const GtH5DataSpace& dataSpace)
{
#if GT_H5
    GtH5FileManager::FileReference fileReference =
            gtH5FileManager->createFileReference(this);

    if (!fileReference.isPathValid())
    {
        gtError() << "hdf5 file cannot be accessed!"
                  << fileReference.filePath;
        return false;
    }

    // filePath must be unix styled
    QString filePath = QDir::fromNativeSeparators(fileReference.filePath);

    GtH5File::AccessFlag fileFlag = GtH5File::OpenReadWrite;

    // check if file exists
    if (!GtH5File::fileExists(filePath) || !GtH5File::isValidH5File(filePath))
    {
        gtDebug() << "hdf5 file does not exist and will be created!"
                  << filePath;
        fileFlag = GtH5File::CreateOverwrite;
    }

    // open file
    file = GtH5File(filePath, fileFlag);

    if (!file.isValid())
    {
        return false;
    }

    // access dataset reference
    GtH5Reference ref(isTemporary() ? fileReference.h5Reference:getH5Reference(),
                      GtH5Reference::ObjectType::DataSet);

    // try retrieving by reference
    if (ref.isValid())
    {
        dataset = ref.toDataSet(file);

        if (dataset.isValid())
        {
            return true;
        }
        gtDebug() << "accessing dataset per refererence failed!";
    }

    // try retrieving by path
    QByteArrayList path = { metaObject()->className(), uuid().toUtf8() };

    // if path does not exist create it
    if (!file.root().exists(path))
    {
        file.root().createGroup(path.at(0));
    }

    // create dataset
    dataset = file.root().createDataset(path.join("/"), dataType, dataSpace);

    if (dataset.isValid())
    {
        // save the reference to the dataset
        setH5Reference(dataset.toReference());
        return true;
    }

    return false;
#else
    return false;
#endif
}

int
GtExternalizedH5Object::getH5Reference() const
{
    return m_reference;
}

void
GtExternalizedH5Object::setH5Reference(int data)
{
#if GT_H5
    // dont set property if object uses temp files
    if (isTemporary() && gtH5FileManager->updateFileReference(this, data))
    {
        return;
    }
#endif

    m_reference = data;
}

void
GtExternalizedH5Object::setH5Reference(const GtH5Reference& reference)
{
#if GT_H5
    setH5Reference(reference.data());
#endif
}
