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

#include "gth5_data.h"
#include "gth5_dataset.h"

class GtExternalizedObject;
/**
 * @brief The GtH5ExternalizeHelper class.
 * Helper class for reading and writing objects to the HDF5 format.
 */
class GT_DATAMODEL_EXPORT GtH5ExternalizeHelper
{
    // error strings for logging
    static const QString ERR_PREFIX;
    static const QString ERR_DATASET_READ;
    static const QString ERR_DATASET_WRITE;

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
     * @return success
     */
    template<typename... T>
    bool write(const GtH5Data<T...>& data,
               QVariant& refVariant) const;

    /**
     * @brief reads the data from the desired dataset
     * @tparam T data types of the dataset to read
     * @param data data object
     * @param refVariant varaint containing the reference to a HDF5 dataset
     * @return success
     */
    template<typename... T>
    bool read(GtH5Data<T...>& data,
              QVariant& refVariant,
              bool fetchInitialVersion) const;

private:

    /// class name of the externalized object
    QString m_objClassName;
    /// uuid of the externalized object
    QString m_objUuid;

    /**
     * @brief helper method to fetch the desired dataset (writing purpose)
     * @param dataType hdf5 datatype
     * @param dataSpace hdf5 dataspace
     * @param refVariant varaint containing the reference to a HDF5 dataset
     * @return datatset (invalid if operation failed)
     */
    GtH5DataSet createDataSet(const GtH5DataType& dataType,
                              const GtH5DataSpace& dataSpace,
                              QVariant& refVariant) const;

    /**
     * @brief  helper method to fetch the desired dataset (reading purpose)
     * @param refVariant varaint containing the reference to a HDF5 dataset
     * @param fetchInitialVersion whether to fetch the initial version of the
     * dataset (will fetch from backup file)
     * @return datatset (invalid if operation failed)
     */
    GtH5DataSet openDataSet(QVariant& refVariant,
                            bool fetchInitialVersion) const;
};

template<typename... T>
bool GtH5ExternalizeHelper::write(const GtH5Data<T...>& data,
                                  QVariant& refVariant) const
{
    if (data.isEmpty())
    {
        gtError() << ERR_PREFIX << ERR_DATASET_WRITE
                  << QObject::tr("(Empty data provided)");
        return false;
    }

    // open the associated dataset
    auto dataset{createDataSet(data.dataType(), data.dataSpace(), refVariant)};

    if (!dataset.isValid())
    {
        gtError() << ERR_PREFIX << ERR_DATASET_WRITE
                  << QObject::tr("(Dataset creation failed)");
        return false;
    }

    // write the data to the dataset
    if (!dataset.write(data))
    {
        gtError() << ERR_PREFIX << ERR_DATASET_WRITE
                  << QObject::tr("(Writing failed)");
        return false;
    }

    return true;
}

template<typename... T>
bool GtH5ExternalizeHelper::read(GtH5Data<T...>& data,
                                 QVariant& refVariant,
                                 bool fetchInitialVersion) const
{
    // open the associated dataset
    auto dataset{openDataSet(refVariant, fetchInitialVersion)};

    if (!dataset.isValid())
    {
        gtError() << ERR_PREFIX << ERR_DATASET_READ
                  << QObject::tr("(Retrieving dataset failed)");
        return false;
    }

    // read the data from the dataset
    if (!dataset.read(data))
    {
        gtError() << ERR_PREFIX << ERR_DATASET_READ
                  << QObject::tr("(Writing failed)");
        return false;
    }

    return true;
}
#endif

#endif // GTH5EXTERNALIZEHELPER
