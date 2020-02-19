/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSESSION_H
#define GTSESSION_H

#include "gt_core_exports.h"

#include <QPointer>

#include "gt_object.h"

class QJsonObject;
class QFile;
class GtProject;
class GtDataModel;

/**
 * @brief The GtSession class
 */
class GT_CORE_EXPORT GtSession : public GtObject
{
    Q_OBJECT

    friend class GtCoreApplication;
    friend class GtCoreDatamodel;
    friend class GtDataModel;

public:
    /**
     * @brief Returns pointer to current project. NULL if no current project
     * selected.
     * @return Pointer to current project
     */
    GtProject* currentProject() const;

    /**
     * @brief findProject
     * @param id
     * @return
     */
    GtProject* findProject(const QString& id);

    /**
     * @brief projects
     * @return
     */
    QList<GtProject*> projects() const;

    /**
     * @brief projectIds
     * @return
     */
    QStringList projectIds() const;

    /**
     * @brief isNull
     * @return
     */
    bool isValid();

    /**
     * @brief model
     * @return
     */
//    GtDataModel* model();

    /**
     * @brief readInformation
     * @param file
     * @return
     */
    virtual QString sessionFilePath();

    /**
     * @brief roamingPath
     * @return
     */
    static QString roamingPath();

    /**
     * @brief ~GtSession
     */
    virtual ~GtSession();

protected:
    /**
     * @brief GtSession
     * @param id
     */
    GtSession(const QString& id);

    /**
     * @brief GtSession
     */
    GtSession();

    /**
     * @brief loadProjectData
     * @param project
     * @return
     */
    GtObjectList loadProjectData(GtProject* project);

    /**
     * @brief saveProjectData
     * @param project
     * @return
     */
    bool saveProjectData(GtProject* project);

    /**
     * @brief createDefault
     * @return
     */
    static bool createDefault();

    /**
     * @brief createEmptySession
     * @param id
     * @return
     */
    static bool createEmptySession(const QString id);

    /**
     * @brief duplicateWorkspcae
     * @param id
     * @return
     */
    static bool duplicateSession(const QString source, const QString target);

    /**
     * @brief setCurrentProject
     * @param val
     * @return
     */
    bool setCurrentProject(const QString& id);

    /**
     * @brief setCurrentProject
     * @param project
     * @return
     */
    bool setCurrentProject(GtProject* project);

    /**
     * @brief switchCurrentProject
     */
    void switchCurrentProject();

    /**
     * @brief addProject
     * @param project
     */
    void addProject(GtProject* project);

    /**
     * @brief deleteProject
     * @param project
     * @return
     */
    bool deleteProject(GtProject* project);

    /**
     * @brief projectIndex
     * @param project
     * @return
     */
    int projectIndex(GtProject* project);

    /**
     * @brief toJson
     * @return
     */
    bool toJsonObject();

    /**
     * @brief fromJson
     * @return
     */
    bool fromJsonObject();

private:
    ///
    bool m_valid;

    /// Pointer to current project
    QPointer<GtProject> m_currentProject;

    /// Datamodel
//    GtDataModel* m_model;

private slots:
    void onTreeDataChange();

};

#endif // GTSESSION_H