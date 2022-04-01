/* GTlab - Gas Turbine laboratory
 * Source File: gt_h5filemanager.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 18.08.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTTEMPH5FILEMANAGER_H
#define GTTEMPH5FILEMANAGER_H

#include <QMap>
#include <QSet>
#include <QString>

#include "gt_object.h"

#define gtH5FileManager (GtH5FileManager::instance())

class GtExternalizedH5Object;
class GT_DATAMODEL_EXPORT GtH5FileManager
{
public:

    /**
     * @brief instance
     * @return singleton instance
     */
    static GtH5FileManager* instance();

    /// file name of the main project hdf5 file
    static const QString S_H5_PROJECT_FILENAME;
    /// dir name of temp dir
    static const QString S_TEMP_DIRNAME;

    /**
     * @brief The FileReference struct helper struct for returning the file path
     * and the hdf5 reference associated with an object
     */
    struct FileReference
    {
        /// file path
        QString filePath;
        /// reference to the hdf5 dataset
        int h5Reference;

        /**
         * @brief isPathValid checks if the directory of the file path exists
         * @return whether base direcotry of file exists
         */
        bool isPathValid() const;
    };

    /**
     * @brief The TempFileIndex struct keeps track of the stored objects (uuids)
     * and dataset references
     */
    struct FileIndex
    {
        /// file path
        QString filePath;
        /// maps uuids to hdf5 dataset references
        QMap<QString, int> uuidMap;
    };

    /**
     * @brief projectFileReference
     * @return
     */
    FileReference projectFileReference() const;

    /**
     * @brief createNewTempEntry creates a new entry in the file map
     * @return temp file id
     */
    int createNewTempFile();

    /**
     * @brief removeFile removes the specified entry from the file index
     * map
     * @param temp file id
     * @return success
     */
    bool removeTempFile(int fileId);

    /**
     * @brief commitFile swaps the datasets in the temp file specified and
     * the main project file
     * @param id
     * @param modules
     * @return success
     */
    bool commitObjectsInTempFile(int fileId, const GtObjectList& modules);

    /**
     * @brief updateFileReference updates the hdf5 reference to the dataset
     * of the object and the specified file id.
     * @param obj
     * @param h5Reference
     * @param fileId can be used to access different temp files
     * @return success
     */
    bool updateFileReference(const GtExternalizedH5Object* obj,
                             int h5Reference,
                             int fileId = -1);

    /**
     * @brief createFileReference creates a new entry for the active temp file.
     * @param obj
     * @return file reference object to access filepath and h5 reference
     */
    FileReference createFileReference(GtExternalizedH5Object* obj);

    /**
     * @brief getFileReference returns the entry for the speicified file id. If
     * the entry does not exist the project file entry is used.
     * @param obj
     * @param fileId can be used to access different temp files
     * @return file reference object to access filepath and h5 reference
     */
    FileReference getFileReference(const GtExternalizedH5Object* obj,
                                   int fileId = -1) const;

    /**
     * @brief reset resets the file map, sets the counter to 0 and attempts to
     * delete all temp files.
     * @param project current project object
     * @param projectDir path to the project dir
     */
    void reset(GtObject* project, const QString& projectDir);

private:

    /**
     * @brief GtTempH5FileManager
     */
    GtH5FileManager();

    /// counter for file ids
    int m_idCounter;
    /// maps the file ids to files indicies, where each index keeps track of the
    /// uuids and hdf5 dataset references of the stored objects
    QMap<int, FileIndex> m_fileMap;
    /// object of the active project
    GtObject* m_projectObject;
    /// path to project dir (used for the hdf5 project file)
    QByteArray m_projectPath;
    /// thread of process runner (used to check if an object is temporary)
    QThread* m_processThread;

    /**
     * @brief commitObject
     * @param cloned
     * @param target
     * @return
     */
    bool commitObject(GtExternalizedH5Object* cloned,
                      GtExternalizedH5Object* target);

    /**
     * @brief generateTempFilePath generates a new file name (absolute path).
     * Does not create any file.
     * @return absolute file path
     */
    QString generateTempFilePath() const;

    /**
     * @brief getObjectByUuid searchs in each module tree for objetc with uuid
     * @param uuid uuid to search for
     * @param modules List of module objects
     * @return object or nullptr
     */
    GtObject* getObjectByUuidHelper(const QString& uuid,
                                    const QList<GtObject*>& modules);

};

#endif // GTTEMPH5FILEMANAGER_H
