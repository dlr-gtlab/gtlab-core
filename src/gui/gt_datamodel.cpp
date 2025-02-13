/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_guidatamodel.cpp
 *
 *  Created on: 15.09.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_object.h"
#include "gt_algorithms.h"
#include "gt_application.h"
#include "gt_loadprojecthelper.h"
#include "gt_saveprojecthelper.h"
#include "gt_project.h"
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
        return {};
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
    // check project pointer
    if (!project)
    {
        return false;
    }

    gtDebug() << tr("Loading project '%1'...").arg(project->objectName());

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
    if (project->upgradesAvailable())
    {
        gtWarning() << "(" << project->objectName() << ") "
                     "project needs updates of data structure!"
                     " Run upgrade project data command first.";
        return false;
    }

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

    auto command = gtApp->makeCommand(parent, commandMsg);
    Q_UNUSED(command)

    // append children
    return GtCoreDatamodel::appendChildren(children, parent);
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

    auto command = gtApp->makeCommand(parent, commandMsg);
    Q_UNUSED(command)

    return GtCoreDatamodel::insertChild(child, parent, row);
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

    auto* commonParent = qobject_cast<GtObject*>(
        gt::find_lowest_ancestor(objects, gt::get_parent_object)
    );

    if (!commonParent)
    {
        gtWarning().verbose()
            << tr("Failed to find common parent object for deletion! "
                  "Defaulting to current project");
        commonParent = gtApp->currentProject();
    }

    auto command = gtApp->makeCommand(commonParent, commandMsg);
    Q_UNUSED(command)

    // append children
    return GtCoreDatamodel::deleteFromModel(objects);
}

void
GtDataModel::dataChangedToRoot(const QModelIndex& index)
{
    // check index
    if (!index.isValid()) return;

    // temporary index for second column
    QModelIndex tmpIndex = createIndex(index.row(), 1,
                                       index.internalPointer());

    // notify model
    emit dataChanged(index, tmpIndex);
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

    initProjectStates(project);

    // collect project data
    GtObjectList data = helper->loadedObjects();

    helper->deleteLater();

    appendProjectData(project, data);

    gtApp->settings()->setLastProject(project->objectName());

    gtInfo() << project->objectName() << tr("loaded!");

    // analyse project information
    GtProjectAnalyzer analyzer(project);

    if (analyzer.hasIrregularities() && !project->ignoringIrregularities())
    {
        GtProjectAnalyzerDialog dialog(&analyzer);
        dialog.resize(500, 400);

        dialog.exec();

        // set ignore irregularities flag
        project->setIgnoreIrregularities(dialog.ignoreIrregularities());
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
