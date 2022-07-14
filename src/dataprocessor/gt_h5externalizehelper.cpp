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

#include "genh5_file.h"
#include "genh5_reference.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>

namespace
{

/**
 * @brief Trys to dereference the dataset for quick access
 * @param file Hdf5 file
 * @param refVariant metadata containg the reference
 * @return datasaet (invalid if derferencing failed)
 */
inline GenH5::DataSet
dereferenceDataSet(GenH5::File& file, const QVariant& refVariant)
{
    bool success = true;
    auto refData = refVariant.toInt(&success);

    GenH5::Reference ref{refData};
    if (success && ref.isValid())
    {
        try
        {
            return ref.toDataSet(file);
        }
        catch (GenH5::ReferenceException const& )
        {
            // nothing to do here
        }
    }
    return {};
}

/**
 * @brief Writes the dataset reference to the metadata varaint
 * @param dset dataset
 * @param refVariant metadata
 */
inline void
referenceDataSet(const GenH5::DataSet& dset, QVariant& refVariant)
{
    refVariant = static_cast<int>(dset.toReference().alignment());
}

/**
 * @brief checks the version of the dataset. Notifies if dataset version is
 * older than current version
 * @param dset dataset
 */
inline void
checkVersionAttribute(const GenH5::DataSet& dset)
{
    static auto printVersion = [](const GenH5::Version& version,
                                  const GenH5::Version& current,
                                  const QByteArray& path) {
        return "(0x" + QByteArray::number(version.toInt(), 16) + " vs. current "
                "0x" + QByteArray::number(current.toInt(), 16) + "; "
                "Dataset path: '" + path + "')";
    };

    // check version of dataset
    GenH5::Version version{};
    GenH5::Version current = GenH5::Version::current();
    if (dset.hasVersionAttribute())
    {
        version = dset.readVersionAttribute();
    }

    if (version.toInt() < current)
    {
        gtWarning() << "HDF5 Dataset was created using an older version of "
                       "GenH5!"
                    << printVersion(version, current, dset.path());
    }
    else if (version.toInt() > current)
    {
        gtWarning() << "HDF5 Dataset was created using a newer version of "
                       "GenH5!"
                    << printVersion(version, current, dset.path());
    }
}

}

GtH5ExternalizeHelper::GtH5ExternalizeHelper(const GtExternalizedObject& obj) :
    m_objClassName(obj.metaObject()->className()),
    m_objUuid(obj.uuid())
{

}

GenH5::File
GtH5ExternalizeHelper::openFile(GenH5::FileAccessFlags flags,
                                bool accessBackupFile) const noexcept(false)
{
    auto filePath = gtH5ExternalizationInterface->projectHDF5FilePath(
                accessBackupFile);

    if (filePath.isEmpty())
    {
        throw GenH5::FileException{"Invalid HDF5 file path"};
    }

    return GenH5::File{filePath.toUtf8(), flags};
}

GenH5::DataSet
GtH5ExternalizeHelper::overwriteDataSet(const GenH5::DataType& dataType,
                                        const GenH5::DataSpace& dataSpace,
                                        QVariant& refVariant
                                        ) const noexcept(false)
{
    GenH5::File file = openFile({ GenH5::Create | GenH5::Open});

    // try retrieving by h5 reference
    auto dset = dereferenceDataSet(file, refVariant);
    if (!dset.isValid())
    {
        // try retrieving by path
        auto group =  file.root().createGroup(m_objClassName.toUtf8());
        dset = group.createDataset(m_objUuid.toUtf8(), dataType, dataSpace);

        // update ref
        referenceDataSet(dset, refVariant);
    }
    // update version attribute
    dset.createVersionAttribute();
    return dset;
}

GenH5::DataSet
GtH5ExternalizeHelper::openDataSet(QVariant& refVariant,
                                   bool fetchInitialVersion
                                   ) const noexcept(false)
{
    GenH5::File file = openFile({GenH5::Open | GenH5::ReadOnly},
                                fetchInitialVersion);

    // try retrieving by h5 reference
    auto dset = dereferenceDataSet(file, refVariant);
    if (dset.isValid())
    {
        // try retrieving by path
        QStringList path{m_objClassName, m_objUuid};
        dset = file.root().openDataset(path.join('/').toUtf8());

        // update ref
        referenceDataSet(dset, refVariant);
    }

    checkVersionAttribute(dset);
    return dset;
}
#endif
