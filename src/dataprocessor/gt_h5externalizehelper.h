/* GTlab - Gas Turbine laboratory
 * Source File: gt_h5externalizehelper.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 01.03.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTH5EXTERNALIZEHELPER
#define GTH5EXTERNALIZEHELPER

#ifdef GT_H5
#include "gt_datamodel_exports.h"
#include "gt_logging.h"
#include "gt_logging/qt_bindings.h"

#include "genh5_version.h"
#include "genh5_data.h"
#include "genh5_dataset.h"
#include "genh5_file.h"

class GtExternalizedObject;
/**
 * @brief The GtH5ExternalizeHelper class.
 * Helper class for reading and writing objects to the HDF5 format.
 */
class GT_DATAMODEL_EXPORT GtH5ExternalizeHelper
{
public:

    /**
     * @brief GtH5ExternalizeHelper
     * @param obj object to fetch
     */
    explicit GtH5ExternalizeHelper(const GtExternalizedObject& obj);

    /**
     * @brief writes the data to the desired dataset
     * @tparam T data types of the dataset to write
     * @param data data object
     * @param refVariant varaint containing the reference to a HDF5 dataset
     * @return success (will throw if operation fails)
     */
    template<typename Data>
    bool write(const Data& data,
               QVariant& refVariant) const noexcept(false);

    /**
     * @brief reads the data from the desired dataset
     * @tparam T data type of the dataset to read
     * @param data data object
     * @param refVariant varaint containing the reference to a HDF5 dataset
     * @return success (will throw if operation fails)
     */
    template<typename Data>
    bool read(Data& data,
              QVariant& refVariant,
              bool fetchInitialVersion) const noexcept(false);

    /**
     * @brief Opens the associated HDF5 file
     * @param flags Access flags of the file
     * @param accessBackupFile whether to fetch the backup or main project file
     * @return file (will throw if operation fails)
     */
    GenH5::File openFile(GenH5::FileAccessFlags flags,
                         bool accessBackupFile = false) const noexcept(false);

    /**
     * @brief helper method to fetch the desired dataset to (over)write
     * @param dataType hdf5 datatype
     * @param dataSpace hdf5 dataspace
     * @param refVariant varaint containing the reference to a HDF5 dataset
     * @return datatset (will throw if operation fails)
     */
    GenH5::DataSet overwriteDataSet(const GenH5::DataType& dataType,
                                    const GenH5::DataSpace& dataSpace,
                                    QVariant& refVariant) const noexcept(false);

    /**
     * @brief  helper method to fetch the desired dataset (reading purpose only)
     * @param refVariant varaint containing the reference to a HDF5 dataset
     * @param fetchInitialVersion whether to fetch the initial version of the
     * dataset (will fetch from backup file)
     * @return datatset (will throw if operation fails)
     */
    GenH5::DataSet openDataSet(QVariant& refVariant,
                               bool fetchInitialVersion) const noexcept(false);

private:

    /// class name of the externalized object
    QString m_objClassName;
    /// uuid of the externalized object
    QString m_objUuid;
};

template<typename Data>
inline bool
GtH5ExternalizeHelper::write(const Data& data,
                             QVariant& refVariant) const noexcept(false)
{
    if (data.isEmpty())
    {
        gtWarning() << QObject::tr("HDF5: Could not write to the dataset!")
                    << QObject::tr("(Empty data provided)");
        return false;
    }

    gtDebug() << "WRITING DATA" << data.raw().toStdVector();

    // open the associated dataset
    auto dset = overwriteDataSet(data.dataType(), data.dataSpace(), refVariant);

    // write the data to the dataset
    if (!dset.write(data))
    {
        gtError() << QObject::tr("HDF5: Could not write to the dataset!")
                  << QObject::tr("(Writing failed)");
        return false;
    }

    return true;
}

template<typename Data>
inline bool
GtH5ExternalizeHelper::read(Data& data,
                            QVariant& refVariant,
                            bool fetchInitialVersion) const noexcept(false)
{
    // open the associated dataset
    auto dset = openDataSet(refVariant, fetchInitialVersion);

    // read the data from the dataset
    if (!dset.read(data))
    {
        gtError() << QObject::tr("HDF5: Could not read from the dataset!")
                  << QObject::tr("(Writing failed)");
        return false;
    }

    return true;
}
#endif

#endif // GTH5EXTERNALIZEHELPER
