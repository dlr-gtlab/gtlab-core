/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROJECT_H
#define GTPROJECT_H

#include "gt_core_exports.h"

#include <QStringList>

#include "gt_object.h"
#include "gt_session.h"
#include "gt_stringproperty.h"

class QDomDocument;
class QDomElement;
class GtProcessData;
class GtTask;
class GtProjectProvider;
class GtPackage;
class GtLabelData;
class GtLabel;

/**
 * @brief The GtProject class
 */
class GT_CORE_EXPORT GtProject : public GtObject
{
    Q_OBJECT

    friend class GtSession;
    friend class GtProjectProvider;
    friend class GtLoadProjectHelper;

public:
    /**
     * @brief ~GtProject
     */
    virtual ~GtProject();

    /**
     * @brief Returns path to project directory on hard drive
     * @return path to project directory
     */
    const QString& path();

    /**
     * @brief Returns whether project data is successfully loaded or not
     * @return validation indicator
     */
    bool isValid();

    /**
     * @brief isOpen
     * @return
     */
    bool isOpen();

    /**
     * @brief processData
     * @return
     */
    GtProcessData* processData();

    /**
     * @brief Returns project specific label data.
     * @return Label data.
     */
    GtLabelData* labelData();

    /**
     * @brief findProcess
     * @param val
     * @return
     */
    GtTask* findProcess(const QString& val);

    /**
     * @brief findPackage
     * @param val
     * @return
     */
    GtPackage* findPackage(const QString& val);

    /**
     * @brief moduleIds
     * @return
     */
    const QStringList& moduleIds();

    /**
     * @brief Returns number of label usages. Returns -1 if given label is
     * coruppted.
     * @param Label.
     * @return Number of label usages.
     */
    int numberOfLabelUsages(GtLabel* label);

    /**
     * @brief Returns all label usages.
     * @param Label.
     * @return Label usages.
     */
    QList<GtLabel*> findLabelUsages(GtLabel* label);

    /**
     * @brief toProjectDataMemento
     * @return
     */
    GtObjectMemento toProjectDataMemento();

    /**
     * @brief fromProjectDataMemento
     * @param memento
     * @return
     */
    bool fromProjectDataMemento(GtObjectMemento& memento);

    /**
     * @brief Renames project identification string within application and
     * project save file. If other project with same id exists, renaming fails.
     * @param str New projcet identification string
     * @return Whether project was renamed or not.
     */
    bool renameProject(const QString& str);

protected:
    /**
     * @brief GtProject
     * @param path
     */
    GtProject(const QString& path);

    /**
     * @brief setModuleIds
     * @param list
     */
    void setModuleIds(const QStringList& list);

    /**
     * @brief Returns filename of main GTlab project file
     * @return
     */
    static const QString mainFilename();

    /**
     * @brief Returns filename of main GTlab project file
     * @return
     */
    static const QString mainFileExtension();

    /**
     * @brief moduleExtension
     * @return
     */
    static const QString moduleExtension();

private:
    /// Path to project directory on hard drive
    QString m_path;

    /// True if project data was successfully loaded
    bool m_valid;

    /// List of all project module ids
    QStringList m_moduleIds;

    /// Project path property. Only used for visualization in the GUI.
    GtStringProperty m_pathProp;

    /**
     * @brief loadMetaData
     * @return
     */
    bool loadMetaData();

    /**
     * @brief readModuleMetaData
     */
    void readModuleMetaData(QDomElement& root);

    /**
     * @brief readProcessData
     * @return
     */
    GtObject* readProcessData();

    /**
     * @brief readLabelData
     * @return
     */
    GtObject* readLabelData(GtObjectList& moduleData);

    /**
     * @brief readModuleData
     * @return
     */
    GtObjectList readModuleData();

    /**
     * @brief saveModuleData
     * @return
     */
    bool saveModuleData();

    /**
     * @brief saveMetaAndProcessData
     * @return
     */
    bool saveProjectOverallData();

    /**
     * @brief saveModuleMetaData
     * @param root
     * @param doc
     * @return
     */
    bool saveModuleMetaData(QDomElement& root, QDomDocument& doc);

    /**
     * @brief saveProcessData
     * @param root
     * @param doc
     * @return
     */
    bool saveProcessData(QDomElement& root, QDomDocument& doc);

    /**
     * @brief saveLabelData
     * @param root
     * @param doc
     * @return
     */
    bool saveLabelData(QDomElement& root, QDomDocument& doc);

    /**
     * @brief readProjectData
     * @return
     */
    QDomElement readProjectData();

    /**
     * @brief findLabelUsagesHelper
     * @param obj
     * @param label
     * @return
     */
    QList<GtLabel*> findLabelUsagesHelper(GtObject* obj, GtLabel* label);

    /**
     * @brief renameOldModuleFile
     * @param path
     * @param modId
     */
    void renameOldModuleFile(const QString& path, const QString& modId);

    /**
     * @brief save project files
     * @param filePath : file path
     * @param doc : document
     * @return success
     */
    bool saveProjectFiles(const QString& filePath, const QDomDocument& doc);

};

#endif // GTPROJECT_H
