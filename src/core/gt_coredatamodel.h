/* GTlab - Gas Turbine laboratory
 * Source File: gt_coredatamodel.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 02.09.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCOREDATAMODEL_H
#define GTCOREDATAMODEL_H

#include "gt_core_exports.h"
#include "gt_qtutilities.h"

#include <QAbstractItemModel>
#include <QPointer>
#include <QObject>

#define gtDataModel (GtCoreDatamodel::instance())

class GtSession;
class GtProject;
class GtObject;
class GtAbstractObjectFactory;

/**
 * @brief The GtCoreDatamodel class
 */
class GT_CORE_EXPORT GtCoreDatamodel : public QAbstractItemModel
{
    friend class GtSession;
    friend class GtCoreApplication;

    Q_OBJECT

public:
    enum DataModelRole {
        UuidRole = Qt::UserRole + 1
    };

    /**
     * @brief Returns current instance of data model.
     * @return Current instance of data model
     */
    static GtCoreDatamodel* instance();

    /**
     * @brief Returns current session.
     * @return Current session.
     */
    GtSession* session();

    /**
     * @brief Returns current project. If no current project exists NULL will
     * be returned.
     * @return Current project pointer
     */
    GtProject* currentProject();

    /**
     * @brief Returns project corresponding to given identification string.
     * Return NULL if no project was found.
     * @param Identification string
     * @return Project pointer
     */
    GtProject* findProject(const QString& id);

    /**
     * @brief Returns list of all project of current session.
     * @return List of session projects
     */
    QList<GtProject*> projects() const;

    /**
     * @brief Returns list of all project identification strings of current
     * session.
     * @return List of session project identification strings
     */
    QStringList projectIds() const;

    /**
     * @brief Opens project corresponding to given identification string.
     * @param Identification string
     * @return Whether project could be opened or not
     */
    bool openProject(const QString& id);

    /**
     * @brief Opens given project.
     * @param Project pointer.
     * @return Whether project could be opened or not
     */
     virtual bool openProject(GtProject* project);

    /**
     * @brief Saves project corresponding to given identification string.
     * @param Identification string
     * @return Whether project could be saved or not
     */
    bool saveProject(const QString& id);

    /**
     * @brief Saves given project.
     * @param Project pointer
     * @return Whether project could be saved or not
     */
    virtual bool saveProject(GtProject* project);

    /**
     * @brief Closes given project.
     * @param Project pointer
     * @return Whether project could be closed or not
     */
    virtual bool closeProject(GtProject* project);

    /**
     * @brief Deletes given project from session.
     * @param Project pointer
     * @return Whether project could be deleted from session or not
     */
    bool deleteProject(GtProject* project);

    /**
     * @brief Adds given project to current session.
     * @param Project pointer
     * @param doOpen Whether to open the project after adding the project
     * @return Whether project could be added or not
     */
    bool newProject(GtProject* project, bool doOpen);

    /**
     * @brief Adds given project to current session and opens it
     *
     * See: newProject(project, doOpen)
     */
    [[deprecated("Use newProject(project, doOpen) instead")]]
    bool newProject(GtProject* project);

    /**
     * @brief Returns the number of columns for the children of the given
     * parent.
     * @param Parent model index
     * @return Number of columns
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Returns the number of rows under the given parent. When the
     * parent is valid it means that rowCount is returning the number of
     * children of parent.
     * @param Parent model index
     * @return Number of rows
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Returns the index of the item in the model specified by the
     * given row, column and parent index.
     * @param Row
     * @param Column
     * @param Parent model index
     * @return Model index
     */
    QModelIndex index(int row,
                      int col,
                      const QModelIndex& parent =
                      QModelIndex()) const override;

    /**
     * @brief Returns the parent of the model item with the given index. If the
     * item has no parent, an invalid QModelIndex is returned.
     * @param Model index
     * @return Parent model index
     */
    QModelIndex parent(const QModelIndex& index) const override;

    /**
     * @brief Returns the data stored under the given role for the item
     * referred to by the index.
     * @param Model index
     * @param Data role
     * @return Data stored in variant
     */
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    /**
     * @brief Sets the role data for the item at index to value.
     * @param Model index
     * @param Value
     * @param Data role
     * @return Returns true if successful; otherwise returns false.
     */
    bool setData(const QModelIndex& index,
                 const QVariant& value,
                 int role = Qt::EditRole) override;

    /**
     * @brief mimeTypes
     * @return
     */
    QStringList mimeTypes() const override;

    /**
     * @brief Returns an object that contains serialized items of data
     * corresponding to the list of indexes specified.
     * @param List of model indexes
     * @return MimeData
     */
    QMimeData* mimeData(const QModelIndexList& indexes) const override;

    /**
     * @brief Creates mime data based ob given object.
     * @param Object
     * @param Whether object should have a new uuid or not
     * @return Mime data of given object
     */
    QMimeData* mimeDataFromObject(GtObject* obj, bool newUuid = false) const;

    /**
     * @brief Creates object from given mime data.
     * @param Mime data
     * @param Whether object should have a new uuid or not
     * @param Factory for object creation
     * @return Recreated object
     */
    GtObject* objectFromMimeData(const QMimeData* mime,
                                 bool newUuid = false,
                                 GtAbstractObjectFactory* factory = nullptr);

    /**
     * @brief Returns object corresponding to given index.
     * @param index of object
     * @return object from index
     */
    virtual GtObject* objectFromIndex(const QModelIndex& index) const;

    /**
     * @brief Returns index corresponding to given object.
     * @param object of index
     * @param column
     * @return index from object
     */
    virtual QModelIndex indexFromObject(GtObject* obj, int col = 0) const ;

    /**
     * @brief Appends given child to given parent object.
     * @param Child object
     * @param Parent object
     * @return ModelIndex of appended child object
     */
    QModelIndex appendChild(GtObject* child, GtObject* parent);

    /**
     * @brief Appends given children to given parent object.
     * @param List of child objects
     * @param Parent object
     * @return ModelIndexList of appended child objects
     */
    virtual QModelIndexList appendChildren(const QList<GtObject*>& children,
                                           GtObject* parent);

    /**
     * @brief Inserts given child object to given paren object on specified
     * position
     * @param Child object
     * @param Parent object
     * @param Target row
     * @return ModelIndex of inserted child object
     */
    virtual QModelIndex insertChild(GtObject* child, GtObject* parent, int row);

    /**
     * @brief Deletes given object from datamodel.
     * @param Object
     * @return Whether object was deleted or not
     */
    virtual bool deleteFromModel(GtObject* obj);

    /**
     * @brief Deletes given objects sfrom datamodel.
     * @param Object list
     * @return Whether all object were deleted or not
     */
    virtual bool deleteFromModel(QList<GtObject *> objects);

    /**
     * @brief Creates an unique object name based on given initial string and
     * a parent object.
     * @param Initial object name
     * @param Parent object
     * @return Unique object name
     */
    QString uniqueObjectName(const QString& name,
                             GtObject* parent);

    /**
     * @brief Creates an unique object name based on given initial string and
     * a parent model index.
     * @param Initial object name
     * @param Parent model index
     * @return Unique object name
     */
    QString uniqueObjectName(const QString& name,
                             const QModelIndex& parent);

    /**
     * @brief Returns object of given path. If no object is found null pointer
     * will be returned
     * @param Object path
     * @return Object
     */
    GtObject* objectByPath(const QString& objectPath);

    /**
     * @brief Returns object for given uuid. If no object is found null pointer
     * will be returned
     * @param Object UUID
     * @return Object
     */
    GtObject* objectByUuid(const QString& objectUuid);

protected:
    /// Pointer to singleton object
    static GtCoreDatamodel* m_self;

    /**
     * @brief Constructor
     * @param parent
     */
    explicit GtCoreDatamodel(QObject* parent = nullptr);

    /**
     * @brief Initialization
     */
    void init();

    /**
     * @brief Initializes project specific states.
     * @param project Loaded project.
     */
    void initProjectStates(GtProject* project);

    /**
     * @brief Returns the item flags for the given index.
     * @param Model index
     * @return Item Flags
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    /**
     * @brief Returns the root parent index for given model index.
     * @param Model index
     * @return Root parent
     */
    QModelIndex rootParent(const QModelIndex& index) const;

    /**
     * @brief appendProjectData
     * @param project
     * @param projectData
     */
    void appendProjectData(GtProject* project,
                           const QList<GtObject*>& projectData);

private:
    /// Current session
    QPointer<GtSession> m_session;

    /**
     * @brief Sets new session.
     * @param New session
     */
    void setSession(GtSession* session);

signals:
    /**
     * @brief Emitted after successful project save.
     * @param project Saved project.
     */
    void projectSaved(GtProject* project);

};

template<typename ObjectList, typename GetNameFunc>
[[deprecated("Use gt::detail::makeUniqueNameImpl instead")]]
QString _getUniqueName_impl(const QString& name,
                            const ObjectList& objs,
                            GetNameFunc getName,
                            QString initName,
                            int iteration)
{
    return gt::detail::makeUniqueNameImpl(
                name, objs, std::move(getName), std::move(initName));
}

/**
 * @brief Returns a unique name given a list of objects names
 *
 * @param name The base name. If e.g. "aa" already exists, "aa[1]" is returned
 * @param objs List of objects to query from
 * @param func A function to get the name from an object
 * @return A unique name
 */
template<typename ObjectList, typename GetNameFunc>
[[deprecated("Use gt::makeUniqueName instead")]]
QString getUniqueName(const QString& name,
                      const ObjectList& objs,
                      GetNameFunc getName)
{
    return gt::makeUniqueName(name, objs, getName);
}

/**
 * @brief Returns a unique name given a list of names
 *
 * @param name The base name. If e.g. "aa" already exists, "aa[1]" is returned
 * @param names List of names to query from
 * @return A unique name
 */
template<typename StringList>
[[deprecated("Use gt::makeUniqueName instead")]]
QString getUniqueName(const QString& name,
                      const StringList& names)
{
    return gt::makeUniqueName(name, names);
}

#endif // GTCOREDATAMODEL_H
