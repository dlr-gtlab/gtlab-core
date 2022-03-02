/* GTlab - Gas Turbine laboratory
 * Source File: gt_guidatamodel.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 15.09.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTDATAMODEL_H
#define GTDATAMODEL_H

#include "gt_gui_exports.h"

#include "gt_coredatamodel.h"

#if defined(gtDataModel)
#undef gtDataModel
#endif
#define gtDataModel (static_cast<GtDataModel *>(GtCoreDatamodel::instance()))

class GtAbstractProperty;

/**
 * @brief The GtGuiDataModel class
 */
class GT_GUI_EXPORT GtDataModel : public GtCoreDatamodel
{
    friend class GtApplication;

    Q_OBJECT

public:
    /**
     * @brief Notifies the model about changes on given object.
     * @param Object
     */
    void updateObject(GtObject* obj);

    /**
     * @brief beginResetModelView
     */
    void beginResetModelView();

    /**
     * @brief endResetModelView
     */
    void endResetModelView();

    /**
     * @brief Closes given project.
     * @param Project pointer
     * @return Whether project could be closed or not
     */
    bool closeProject(GtProject* project) override;

    /**
     * @brief Returns the number of columns for the children of the given
     * parent.
     * @param Parent model index
     * @return Number of columns
     */
    int columnCount(const QModelIndex& parent =
            QModelIndex()) const override;

    /**
     * @brief Opens given project.
     * @param Project pointer.
     * @return Whether project could be opened or not
     */
     bool openProject(GtProject* project) override;

     /**
      * @brief Saves given project.
      * @param Project pointer.
      * @return Whether project could be saved or not
      */
      bool saveProject(GtProject* project) override;

    /**
     * @brief Appends given children to given parent object.
     * @param List of child objects
     * @param Parent object
     * @return ModelIndexList of appended child objects
     */
    QModelIndexList appendChildren(const QList<GtObject*>& children,
                                   GtObject* parent) override;

    /**
     * @brief insertChild
     * @param child
     * @param parent
     * @param row
     * @return
     */
    QModelIndex insertChild(GtObject* child,
                            GtObject* parent,
                            int row) override;

    /**
     * @brief Deletes given object from datamodel.
     * @param Object
     * @return Whether object was deleted or not
     */
    bool deleteFromModel(GtObject* obj) override;

    /**
     * @brief Deletes given objects sfrom datamodel.
     * @param Object list
     * @return Whether all object were deleted or not
     */
    bool deleteFromModel(QList<GtObject*> objects) override;

    /**
     * @brief reduceToParents
     * @param toReduce
     */
    void reduceToParents(QList<GtObject*>& toReduce);

protected:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit GtDataModel(QObject* parent = nullptr);

    /**
     * @brief Returns the drop actions supported by this model.
     * @return Supported drop actions
     */
    Qt::DropActions supportedDropActions() const override;

    /**
     * @brief Returns the actions supported by the data in this model.
     * @return Supported drag actions
     */
    Qt::DropActions supportedDragActions() const override;

    /**
     * @brief Returns the item flags for the given index.
     * @param Model index
     * @return Item Flags
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    /**
     * @brief dataChangedToRoot
     * @param index
     */
    void dataChangedToRoot(const QModelIndex& index);

private slots:
    /**
     * @brief onObjectDataChanged
     * @param obj
     */
    void onObjectDataChanged(GtObject* obj);

    /**
     * @brief onObjectDataChanged
     */
    void onObjectDataChanged(GtObject* obj, GtAbstractProperty* prop);

    /**
     * @brief onProjectDataLoaded
     */
    void onProjectDataLoaded();

    /**
     * @brief onProjectDataSaved emits projectSaved when saving was successfull
     */
    void onProjectDataSaved();

signals:
    /**
     * @brief triggerBeginResetDataModel
     */
    void triggerBeginResetDataModelView();

    /**
     * @brief triggerEndResetDataModelView
     */
    void triggerEndResetDataModelView();

};

#endif // GTDATAMODEL_H
