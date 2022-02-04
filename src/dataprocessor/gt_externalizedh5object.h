/* GTlab - Gas Turbine laboratory
 * Source File: gt_externalizedh5object.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 04.08.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTEXTERNALIZEDH5OBJECT_H
#define GTEXTERNALIZEDH5OBJECT_H

#include "gt_datamodel_exports.h"
#include "gt_externalizedobject.h"

class GtH5File;
class GtH5DataSet;
class GtH5DataSpace;
class GtH5DataType;
class GtH5Reference;
/**
 * @brief The GtExternalizedH5Object class
 */
class GT_DATAMODEL_EXPORT GtExternalizedH5Object : public GtExternalizedObject
{    
    Q_OBJECT

    Q_PROPERTY(int h5Reference READ getH5Reference WRITE setH5Reference)

public:

    /**
     * @brief tempFileId id of the temp file
     * @return
     */
    int h5FileIndex() const;

protected:

    /**
     * @brief GtExternalizedH5Object
     */
    Q_INVOKABLE GtExternalizedH5Object();

    /**
     * @brief doFetch method to implement for fetching the externalized data.
     * @return success
     */
    virtual bool doFetchData() override = 0;

    /**
     * @brief doExternalize method to implement for externalizing the data.
     * @return success
     */
    virtual bool doExternalizeData() override = 0;

    /**
     * @brief doClearExternalizedData method to implement for clearing the
     * externalized data. Should not emit a signal or set any object flags.
     */
    virtual void doClearExternalizedData() override = 0;

    /**
     * @brief getDataSet attempts opens the dataset associated with this
     * object and returns the file and dataset as reference. Dataset is opened
     * in ReadOnly mode and cannot be modified. File reference must stay in
     * scope as long as dataset is accessed!
     * @param file reference to the hdf5 file.
     * @param dataset reference to the hdf5 dataset.
     * @return success
     */
    bool getDataSet(GtH5File& file, GtH5DataSet& dataset);

    /**
     * @brief createDataSet attempts to open the dataset associated with this
     * object and returns the file and dataset as reference. If dataset or file
     * does not exist it will be created. Dataset is opened in ReadWrite mode.
     * File reference must stay in scope as long as dataset is accessed!
     * @param file reference to the hdf5 file.
     * @param dataset reference to the hdf5 dataset.
     * @param dataType the datatype for the dataset.
     * @param dataSpace the dataspace (layout and dimensions) for the data of
     * the dataset.
     * @return success
     */
    bool createDataSet(GtH5File& file,
                       GtH5DataSet& dataset,
                       const GtH5DataType& dataType,
                       const GtH5DataSpace& dataSpace);

    /**
     * @brief getH5Reference getter for the reference property.
     * @return
     */
    int getH5Reference() const;

    /**
     * @brief setH5Reference setter for the reference property.
     * @param data
     */
    void setH5Reference(int data);

    /**
     * @brief setH5Reference overloaded setter for the reference property.
     * @param reference
     */
    void setH5Reference(const GtH5Reference& reference);

private:

    /// holds a reference to the hdf5 dataset for quick access
    int m_reference;
};

#endif // GTEXTERNALIZEDH5OBJECT_H
