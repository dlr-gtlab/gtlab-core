/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 29.06.20106
 *  Author: Martin Siggel (SC-IVS)
 *  Tel.: +49 2203 601 2264
 */

#include "gt_projectdiffpatch.h"

//#include "gt_objectfactory.h"
//#include "gt_objectmemento.h"
//#include "gt_objectmementodiff.h"
//#include "gt_logging.h"
//#include "gt_project.h"

//#include <QString>

//QString GtProjectDiffPatch::diff(QString oldMementoString, QString newMementoString)
//{
//    QString errorMsg;

//    // restore old memento from string
//    GtObjectMemento oldMemento(oldMementoString.toUtf8());

//    GtObjectMemento newMemento(newMementoString.toUtf8());

//    GtObjectMementoDiff mementoDiff(oldMemento, newMemento);
//    return QString::fromUtf8(mementoDiff.toByteArray());
//}

//bool GtProjectDiffPatch::patch(QString oldMementoString, QString mementoDiffString, QString &resultMementoString)
//{
//    QString errorMsg;

//    // restore old memento from string
//    GtObjectMemento oldMemento(oldMementoString.toUtf8());

//    // restore mementodiff from string
//    GtObjectMementoDiff mementoDiff(mementoDiffString.toUtf8());

//    // Create Project from memento
//    GtObject* newProject = oldMemento.restore<GtObject*>(gtObjectFactory);
//    if (!newProject)
//    {
//        gtError() << "Cannot create project from memento";
//        return false;
//    }

//    // Apply mementodiff to project
//    if (!newProject->applyDiff(mementoDiff)) {
//        gtError() << "Cannot apply diff to project";
//        delete newProject;
//        return false;
//    }

//    // Create memento string from project
//    resultMementoString = QString::fromUtf8(newProject->toMemento().toByteArray());
//    return true;

//}
