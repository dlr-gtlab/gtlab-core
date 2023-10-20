/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_h5externalizationinterface.h
 *
 *  Created on: 31.03.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTH5EXTERNALIZATIONINTERFACE
#define GTH5EXTERNALIZATIONINTERFACE

#ifdef GT_H5
#include "gt_abstractexternalizationinterface.h"

#define gtH5ExternalizationInterface (GtH5ExternalizationInterface::instance())

/**
 * @brief The GtH5ExternalizationInterface class.
 * Used when externalizing objects to the HDF5 format.
 * Manages the main project HDF5 file. Creates a backup file when a project is
 * loaded, which is used t retrieve the initial version of each dataset,
 * while the main project file contains the modified datasets.
 */
class GT_DATAMODEL_EXPORT GtH5ExternalizationInterface :
        public GtAbstractExternalizationInterface
{
    Q_OBJECT

    /// base file name of the hdf5 project file
    static const QString S_PROJECT_BASENAME;
    /// file suffix indicating a backup file
    static const QString S_BACKUP_SUFFIX;

public:

    /**
     * @brief instance
     * @return singleton instance
     */
    static GtH5ExternalizationInterface* instance();

    /**
     * @brief method to run, when project was loaded. Handles the backup
     * process of the hdf5 file.
     */
    void onProjectLoaded(const QString& projectDir) override;

    /**
     * @brief returns the file path to the hdf5 file.
     * @param useBackupFile whether to sue the backup file or main project file.
     * The backup file should not be modified.
     * @return file path to hdf5 file
     */
    QString projectHDF5FilePath(bool useBackupFile) const;

private:

    /**
     * @brief GtH5ExternalizationInterface
     */
    GtH5ExternalizationInterface();
};
#endif

#endif // GTH5EXTERNALIZATIONINTERFACE
