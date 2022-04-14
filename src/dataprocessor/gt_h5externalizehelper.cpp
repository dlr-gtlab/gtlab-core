/* GTlab - Gas Turbine laboratory
 * Source File: gt_h5externalizehelper.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 01.03.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_h5externalizehelper.h"

#ifdef GT_H5
#include "gt_externalizedobject.h"
#include "gt_h5externalizationinterface.h"

#include "gth5_file.h"
#include "gth5_reference.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>



const QString GtH5ExternalizeHelper::ERR_PREFIX =
        QObject::tr("HDF5:");
const QString GtH5ExternalizeHelper::ERR_DATASET_WRITE =
        QObject::tr("Could not write to the dataset!");
const QString GtH5ExternalizeHelper::ERR_DATASET_READ =
        QObject::tr("Could not read from the dataset!");

GtH5ExternalizeHelper::GtH5ExternalizeHelper(const GtExternalizedObject& obj) :
    m_objClassName(obj.metaObject()->className()),
    m_objUuid(obj.uuid())
{

}

GtH5DataSet
GtH5ExternalizeHelper::createDataSet(const GtH5DataType& dataType,
                                     const GtH5DataSpace& dataSpace,
                                     QVariant& refVariant) const
{
    QString filePath{gtH5ExternalizationInterface->h5ProjectFilePath(false)};

    if (filePath.isEmpty())
    {
        gtError() << ERR_PREFIX
                  << QObject::tr("Invalid filepath for externalizing object!");
        return {};
    }

    auto flag = GtH5File::AccessFlag::OpenReadWrite;

    if (!GtH5File::fileExists(filePath))
    {
        gtDebug() << ERR_PREFIX
                  << QObject::tr("File does not exist and will be created!")
                  << filePath;

        flag = GtH5File::CreateOverwrite;
    }

    auto file = GtH5File(filePath, flag);

    if (!file.isValid())
    {
        gtError() << ERR_PREFIX
                  << QObject::tr("Failed to open/create HDF5 file!")
                  << filePath;
        return {};
    }

    // try retrieving by h5 reference
    bool success{true};
    auto refData = refVariant.toInt(&success);

    if (success && refData > 0)
    {
        GtH5Reference ref(refData);

        auto dset = ref.toDataSet(file);

        if (dset.isValid())
        {
            return dset;
        }
    }

    // try retrieving by path
    auto group =  file.root().createGroup(m_objClassName);

    auto dataset = group.createDataset(m_objUuid, dataType, dataSpace);

    // update ref
    if (dataset.isValid())
    {
        refVariant = static_cast<int>(dataset.toReference().data());
    }

    return dataset;
}

GtH5DataSet
GtH5ExternalizeHelper::openDataSet(QVariant& refVariant,
                                   bool fetchInitialVersion) const
{
    // get filpath
    QString filePath{gtH5ExternalizationInterface->h5ProjectFilePath(
                     fetchInitialVersion)};

    if (!GtH5File::fileExists(filePath))
    {
        gtError() << ERR_PREFIX
                  << QObject::tr("Invalid filepath for fetching object!");
        return {};
    }

    auto file = GtH5File(filePath, GtH5File::OpenReadWrite);

    if (!file.isValid())
    {
        gtError() << ERR_PREFIX
                  << QObject::tr("Failed to open file!") << filePath;
        return {};
    }

    // try retrieving by h5 reference
    bool success{true};
    auto refData = refVariant.toInt(&success);

    if (success && refData > 0)
    {
        GtH5Reference ref(refData);

        auto dset = ref.toDataSet(file);

        if (dset.isValid())
        {
            return dset;
        }
    }

    // try retrieving by path
    QStringList path = {m_objClassName, m_objUuid};

    auto dataset = file.root().openDataset(path.join('/'));

    // update ref
    if (dataset.isValid())
    {
        refVariant = static_cast<int>(dataset.toReference().data());
    }

    return dataset;
}
#endif
