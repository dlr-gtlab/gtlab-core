/* GTlab - Gas Turbine laboratory
 * Source File: gt_h5externalizehelper.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 01.03.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_h5externalizehelper.h"
#include "gt_version.h"

#ifdef GT_H5
#include "gt_externalizedobject.h"
#include "gt_h5externalizationinterface.h"

#include "genh5_file.h"
#include "genh5_reference.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>

static const auto S_GT_VERSION_ATTR = QByteArrayLiteral("GT_VERSION");
static const auto S_EXT_HASH_ATTR = QByteArrayLiteral("GT_EXT_HASH");

static const GenH5::Version s_currentGTlabVersion{
    GT_VERSION_MAJOR,
    GT_VERSION_MINOR,
    GT_VERSION_PATCH
};

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
checkAttributes(const GenH5::DataSet& dset, QString const& hash)
{
    auto const printVersionDiff = [&](const GenH5::Version& version,
                                      const GenH5::Version& current,
                                      const char* type) {
        using namespace gt::log;

        gtWarning().verbose(version < current ? Silent : Medium)
                << QObject::tr("HDF5 dataset was created using an %1 version of %2! "
                               "(0x%3 vs. current 0x%4; path: %5")
                   .arg(version < current ? "older":"newer",
                        type,
                        QString::number(version.toInt(), 16),
                        QString::number(current.toInt(), 16),
                        dset.path());
    };

    // check version attr of GenH5
    if (dset.hasVersionAttribute())
    {
        GenH5::Version version = dset.readVersionAttribute();
        GenH5::Version current = GenH5::Version::current();

        if (version != current) printVersionDiff(version, current, "GenH5");
    }
    else
    {
        gtWarning()
                << QObject::tr("No GenH5 version attribute on HDF5 dataset found!");
    }

    // check version attr of GTlab
    if (dset.hasVersionAttribute(S_GT_VERSION_ATTR))
    {
        GenH5::Version version = dset.readVersionAttribute(S_GT_VERSION_ATTR);
        GenH5::Version current = s_currentGTlabVersion;

        if (version != current) printVersionDiff(version, current, "GTlab");
    }
    else
    {
        gtWarning().medium()
                << QObject::tr("No GTlab version attribute on HDF5 dataset found!");
    }

    // check hash attr
    if (dset.hasAttribute(S_EXT_HASH_ATTR))
    {
        auto otherHash = dset.readAttribute0D<GenH5::FixedString0D>(S_EXT_HASH_ATTR);

        if (otherHash != hash)
        {
            gtWarning() << QObject::tr("Hash on HDF5 dastaset does not match! "
                                       "(%1 vs. expected %2; path: %3)")
                           .arg(otherHash.value(), hash, dset.path());
        }
    }
    else
    {
        gtWarning().medium()
                << QObject::tr("No Hash attribute on HDF5 dataset found!");
    }
}

inline void
updateAttributes(const GenH5::DataSet& dset, QString const& hash)
{
    // update GenH5 version attribute
    dset.writeVersionAttribute();
    // update GTlab version attribute
    dset.writeVersionAttribute(S_GT_VERSION_ATTR, s_currentGTlabVersion);
    dset.writeAttribute(S_EXT_HASH_ATTR, GenH5::makeFixedStr(hash));
}

inline QString
className(QString const& name)
{
    // class name contains
    int idx = name.indexOf(QStringLiteral("$$"));
    assert (idx >= 0);

    return name.mid(0, idx);
}

inline QString
extHash(QString const& name)
{
    // class name contains
    int idx = name.indexOf(QStringLiteral("$$"));
    assert (idx >= 0);

    return name.mid(idx + 2);
}

}

GtH5ExternalizeHelper::GtH5ExternalizeHelper(const GtExternalizedObject& obj) :
    // we need not only the class name but also the externalized hash
    // as we dont want to cause an ABI incompatibility we engrave the hash
    // into this property
    m_metaData(obj.metaObject()->className() +
               QStringLiteral("$$") +
               obj.extHash()),
    m_objUuid(obj.uuid())
{ }

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
    if (dset.isValid())
    {
        // make sure dataset has enough space
        if (dset.dataSpace() != dataSpace || dset.dataType() != dataType)
        {
            dset.deleteRecursively();
            dset = GenH5::DataSet{};
        }
    }

    // recreate by path
    if (!dset.isValid())
    {

        // try retrieving by path
        auto group =  file.root().createGroup(className(m_metaData).toUtf8());
        dset = group.createDataSet(m_objUuid.toUtf8(), dataType, dataSpace);

        // update ref
        referenceDataSet(dset, refVariant);
    }

    updateAttributes(dset, extHash(m_metaData));
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
    if (!dset.isValid())
    {
        // try retrieving by path
        QStringList path{className(m_metaData), m_objUuid};
        dset = file.root().openDataSet(path.join('/').toUtf8());

        // update ref
        referenceDataSet(dset, refVariant);
    }

    checkAttributes(dset, extHash(m_metaData));
    return dset;
}
#endif
