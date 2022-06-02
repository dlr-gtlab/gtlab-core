/* GTlab - Gas Turbine laboratory
 * Source File: gt_guidatamodel.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 15.09.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_object.h"
#include "gt_objectmementodiff.h"
#include "gt_mementochangecommand.h"
#include "gt_session.h"
#include "gt_application.h"
#include "gt_loadprojecthelper.h"
#include "gt_saveprojecthelper.h"
#include "gt_project.h"
#include "gt_command.h"
#include "gt_settings.h"
#include "gt_projectanalyzer.h"
#include "gt_projectanalyzerdialog.h"

#include "gt_datamodel.h"

GtDataModel::GtDataModel(QObject* parent) : GtCoreDatamodel(parent)
{
}

Qt::DropActions
GtDataModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}

Qt::DropActions
GtDataModel::supportedDragActions() const
{
    return Qt::MoveAction;
}

Qt::ItemFlags
GtDataModel::flags(const QModelIndex& index) const
{
    // check index
    if (!index.isValid())
    {
        return nullptr;
    }

    // collect default flags
    Qt::ItemFlags defaultFlags = GtCoreDatamodel::flags(index);

    // get object
    GtObject* object = objectFromIndex(index);

    // check object
    if (object)
    {
        // add drop enabled flag
        defaultFlags = defaultFlags | Qt::ItemIsDropEnabled;
    }

    // return flags
    return Qt::ItemIsDragEnabled | defaultFlags;

}

void
GtDataModel::updateObject(GtObject* obj)
{
    // check object
    if (!obj)
    {
        return;
    }

    // update model
    onObjectDataChanged(obj);
}

void
GtDataModel::beginResetModelView()
{
    emit triggerBeginResetDataModelView();
}

void
GtDataModel::endResetModelView()
{
    beginResetModel();

    endResetModel();

    emit triggerEndResetDataModelView();
}

bool
GtDataModel::closeProject(GtProject* project)
{
    if (GtCoreDatamodel::closeProject(project))
    {
        gtApp->undoStack()->clear();
        gtApp->settings()->setLastProject(QString());

        return true;
    }

    return false;
}

int
GtDataModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 2;
}

bool
GtDataModel::openProject(GtProject* project)
{
    qDebug() << "GtDataModel::openProject";
    // check project pointer
    if (!project)
    {
        return false;
    }

    // check if project is already open
    if (project->isOpen())
    {
        return false;
    }


    // check whether a project is already open
    if (gtDataModel->currentProject())
    {
        return false;
    }

    // project ready to be opened. check for module updater
    runModuleUpdater(project);

    // loading procedure
    GtLoadProjectHelper* helper = new GtLoadProjectHelper(project);
    connect(helper, SIGNAL(finished()), SLOT(onProjectDataLoaded()));

    gtApp->loadingProcedure(helper);


    return true;
}

bool
GtDataModel::saveProject(GtProject *project)
{
    // check pointer
    if (!project)
    {
        return false;
    }

    // saving procedure
    GtSaveProjectHelper* helper = new GtSaveProjectHelper(project);
    connect(helper, SIGNAL(finished()), SLOT(onProjectDataSaved()));

    gtApp->loadingProcedure(helper);

    return true;
}

QModelIndexList
GtDataModel::appendChildren(const QList<GtObject*>& children, GtObject* parent)
{
    // check child list
    if (children.isEmpty())
    {
        return QModelIndexList();
    }

    // check parent
    if (!parent)
    {
        return QModelIndexList();
    }

    if (!gtApp->currentProject())
    {
        return QModelIndexList();
    }

    QString commandMsg;

    if (children.size() == 1)
    {
        commandMsg = children.first()->objectName() + QStringLiteral(" ") +
                     tr("added");
    }
    else
    {
        commandMsg = QString::number(children.size()) + QStringLiteral(" ") +
                     tr("objects") + QStringLiteral(" ") + tr("added");
    }

    GtCommand command = gtApp->startCommand(gtApp->currentProject(),
                                            commandMsg);

    // append children
    QModelIndexList retval = GtCoreDatamodel::appendChildren(children, parent);

    gtApp->endCommand(command);

    return retval;
}

QModelIndex
GtDataModel::insertChild(GtObject* child, GtObject* parent, int row)
{
    if (!child)
    {
        gtWarning() << tr("Could not insert child!") <<
                    QStringLiteral(" - ") <<
                    QStringLiteral("child == NULL");
        return {};
    }

    // check parent object
    if (!parent)
    {
        gtWarning() << tr("Could not insert child!") <<
                    QStringLiteral(" - ") <<
                    QStringLiteral("parent == NULL");
        return {};
    }

    // get index of parent object
    QModelIndex parentIndex = indexFromObject(parent);

    // check parent index
    if (!parentIndex.isValid())
    {
        gtWarning() << tr("Could not insert children!") <<
                    QStringLiteral(" - ") <<
                    QStringLiteral("parent index invalid");
        return {};
    }

    QString commandMsg = child->objectName() + QStringLiteral(" ") +
                         tr("inserted");

    GtCommand command = gtApp->startCommand(gtApp->currentProject(),
                                            commandMsg);

    QModelIndex retval = GtCoreDatamodel::insertChild(child, parent, row);

    gtApp->endCommand(command);

    return retval;
}

bool
GtDataModel::deleteFromModel(GtObject* obj)
{
    return GtCoreDatamodel::deleteFromModel(obj);
}

bool
GtDataModel::deleteFromModel(QList<GtObject*> objects)
{
    // check object list
    if (objects.isEmpty())
    {
        return true;
    }

    QString commandMsg;

    if (objects.size() == 1)
    {
        commandMsg = objects.first()->objectName() + QStringLiteral(" ") +
                     tr("deleted");
    }
    else
    {
        commandMsg = QString::number(objects.size()) + QStringLiteral(" ") +
                     tr("objects") + QStringLiteral(" ") + tr("deleted");
    }

    reduceToParents(objects);

    GtCommand command = gtApp->startCommand(gtApp->currentProject(),
                                            commandMsg);

    // append children
    bool retval = GtCoreDatamodel::deleteFromModel(objects);

    gtApp->endCommand(command);

    return retval;
}

void
GtDataModel::dataChangedToRoot(const QModelIndex& index)
{
    // check index
    if (!index.isValid())
    {
        return;
    }

//    // temporary index for second column
    QModelIndex tmpIndex = createIndex(index.row(), 1,
                                       index.internalPointer());

//    // notify model
    emit dataChanged(index, tmpIndex);

//    // check parent index
//    if (index.parent().isValid())
//    {
//        // notify model recursively
//        dataChangedToRoot(index.parent());
//    }
    //    emit dataChanged(index, index);
}

void
GtDataModel::reduceToParents(QList<GtObject*>& toReduce)
{
    foreach (GtObject* obj, toReduce)
    {
        GtObject* parObj = obj->parentObject();

        while (parObj != nullptr)
        {
            if (toReduce.contains(parObj))
            {
                toReduce.removeAll(obj);
            }

            parObj = parObj->parentObject();
        }
    }
}

void
GtDataModel::onObjectDataChanged(GtObject* obj)
{
    // check object
    if (!obj)
    {
        return;
    }

    // get index of object
    QModelIndex objIndex = indexFromObject(obj);

    // check index
    if (!objIndex.isValid())
    {
        return;
    }

    // notify model up to root
    dataChangedToRoot(objIndex);
}

void
GtDataModel::onObjectDataChanged(GtObject* obj, GtAbstractProperty* /*prop*/)
{
    onObjectDataChanged(obj);
}

void
GtDataModel::onProjectDataLoaded()
{
    GtLoadProjectHelper* helper = qobject_cast<GtLoadProjectHelper*>(sender());

    if (!helper)
    {
        return;
    }
    GtProject* project = helper->project();

    if (!project)
    {
        helper->deleteLater();
        return;
    }

    // collect project data
    GtObjectList data = helper->loadedObjects();

    helper->deleteLater();

    appendProjectData(project, data);

    initProjectStates(project);

    gtApp->settings()->setLastProject(project->objectName());

    // analyse project information
    GtProjectAnalyzer analyzer(project);

    if (analyzer.hasIrregularities())
    {
        GtProjectAnalyzerDialog dialog(&analyzer);
        dialog.resize(500, 400);

        dialog.exec();
    }
}

void
GtDataModel::onProjectDataSaved()
{
    GtSaveProjectHelper* helper = qobject_cast<GtSaveProjectHelper*>(sender());

    if (!helper)
    {
        return;
    }
    GtProject* project = helper->project();

    if (!project)
    {
        helper->deleteLater();
        return;
    }

    // check if saving was usccessfull
    bool success = helper->success();

    helper->deleteLater();

    // emit project saved event
    if (success)
    {        
        gtInfo() << project->objectName() << tr("saved!");

        emit projectSaved(project);
    }
}
