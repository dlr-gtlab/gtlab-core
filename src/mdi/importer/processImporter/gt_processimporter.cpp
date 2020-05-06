/* GTlab - Gas Turbine laboratory
 * Source File: gt_processimporter.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 04.05.2020
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */

#include <QIcon>
#include <QFile>

#include "gt_taskfactory.h"
#include "gt_task.h"
#include "gt_objectfactory.h"
#include "gt_processdata.h"

#include "gt_processimporter.h"

GtProcessImporter::GtProcessImporter()
{
    setObjectName(tr("Process Element"));
}

QStringList
GtProcessImporter::targets() const
{
    QStringList retVal;

    foreach (GtTaskData taskData,
             gtTaskFactory->taskDataList())
    {
        QString tmpStr = taskData->metaData().className();
        retVal.append(tmpStr);
    }

    retVal.append(GT_CLASSNAME(GtProcessData));

    return retVal;
}

QIcon
GtProcessImporter::icon() const
{
    QIcon icon;
    icon.addFile(QString(":/icons/importIcon_16.png"), QSize(), QIcon::Normal,
                 QIcon::Off);

    return icon;
}

bool
GtProcessImporter::import(GtObject* data, QFile &file)
{
    if (!file.exists())
    {
        gtError() << "File not found!";
        return false;
    }

    if (data == Q_NULLPTR)
    {
        gtError() << "target is nullptr.";
        return false;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        gtError() << "Could not open file" << file.fileName();
        return false;
    }

    QByteArray a = file.readAll();
    GtObjectMemento memento(a);
    GtObject* obj = memento.restore(gtObjectFactory, true);

    if (obj == Q_NULLPTR)
    {
        gtError() << "resorted obj is nullptr.";
        return false;
    }

    /// validate object to import
    if (qobject_cast<GtProcessComponent*>(obj) == Q_NULLPTR)
    {
        gtError() << "resorted obj is not of type GtProcessElement.";
        delete obj;
        return false;
    }

    /// if the import is not done with a task as parent only tasks are
    /// allowed to be improted
    if (qobject_cast<GtTask*>(data) == Q_NULLPTR)
    {
        if (qobject_cast<GtTask*>(obj) == Q_NULLPTR)
        {
            gtError() << "Parent item of a calculator has to be a task";
            delete obj;
            return false;
        }
    }

    data->appendChild(obj);

    return true;
}

QStringList
GtProcessImporter::extensions() const
{
    return QStringList() << QStringLiteral("XML (*.xml)");
}
