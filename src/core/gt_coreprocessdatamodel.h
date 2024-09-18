/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_coredatamodel.h
 *
 *  Created on: 02.09.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCOREPROCESSDATAMODEL_H
#define GTCOREPROCESSDATAMODEL_H

#include "gt_core_exports.h"
#include "gt_processdata.h"

#include <QAbstractItemModel>
#include <QPointer>
#include <QObject>

#define gtProcessDataModel (GtCoreProcessDatamodel::instance())

class GtProcessData;
class GtTaskGroup;
class GtObject;
class GtAbstractObjectFactory;
class GtProject;

/**
 * @brief The GtCoreProcessDatamodel class
 */
class GT_CORE_EXPORT GtCoreProcessDatamodel : public QAbstractItemModel
{
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
    static GtCoreProcessDatamodel* instance();

    /**
     * @brief Returns current project. If no current project exists NULL will
     * be returned.
     * @return Current project pointer
     */
    GtTaskGroup* currentTaskGroup();

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
     * @brief reduceToParents
     * @param toReduce
     */
    void reduceToParents(QList<GtObject*>& toReduce);

    /**
     * @brief Sets new session.
     * @param New session
     */
    void setProcessData(GtProcessData* processData);

    GtProcessData* session();

    /**
     * @brief Initializes project specific states.
     * @param project Loaded project.
     */
    void initProjectStates(GtProject* project);

protected:
    /// Pointer to singleton object
    static GtCoreProcessDatamodel* m_self;

    /**
     * @brief Constructor
     * @param parent
     */
    explicit GtCoreProcessDatamodel(QObject* parent = nullptr);

    /**
     * @brief Initialization
     */
    void init();

    /**
     * @brief Returns the item flags for the given index.
     * @param Model index
     * @return Item Flags
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    /// Current session
    QPointer<GtProcessData> m_processData;


signals:
    /**
     * @brief triggerEndResetDataModelView
     */
    void triggerEndResetDataModelView();
};

#endif // GTCOREPROCESSDATAMODEL_H
