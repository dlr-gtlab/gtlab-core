/* GTlab - Gas Turbine laboratory
 * Source File: gt_scheduleexporter.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 15.03.2018
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_scheduleexporter.h"

#include <QIcon>

#include "gt_logging.h"
#include "gt_controlschedule.h"
#include "gt_objectmemento.h"

GtScheduleExporter::GtScheduleExporter()
{
    setObjectName(tr("SDL Exporter"));
}

QStringList
GtScheduleExporter::targets() const
{
    return QStringList() << GT_CLASSNAME(GtControlSchedule);
}

QIcon
GtScheduleExporter::icon() const
{
    QIcon icon;
    icon.addFile(QString(":/icons/exportIcon_16.png"),
                 QSize(), QIcon::Normal,
                 QIcon::Off);

    return icon;
}

bool
GtScheduleExporter::doExport(GtObject* data, QFile& file)
{
    if (data == Q_NULLPTR)
    {
        gtError() << tr("Data corrupted!");
        return false;
    }

    GtControlSchedule* schedule = qobject_cast<GtControlSchedule*>(data);

    if (schedule == Q_NULLPTR)
    {
        gtError() << tr("Schedule corrupted!");
        return false;
    }

    gtInfo() << tr("exporting ") << data->objectName() << " to "
             << file.fileName();

    if (!writeData(schedule, file))
    {
        gtError() << "Export failed...";
        return false;
    }

    gtInfo() << "DAT export finished succesfull";

    return true;
}

QString
GtScheduleExporter::extension() const
{
    return QStringLiteral("SDL (*.sdl)");
}

QList<GtAbstractExporterPage*>
GtScheduleExporter::pages(GtObject *data)
{
    QList<GtAbstractExporterPage*> retval;

    retval.append(GtAbstractExporter::pages(data));

    return retval;
}

bool
GtScheduleExporter::writeData(GtControlSchedule* data, QFile& file)
{
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gtError() << tr("Could not open output file!") << file.fileName();
        return false;
    }

    QTextStream out(&file);

    if (data == Q_NULLPTR)
    {
        file.close();
        return false;
    }

    GtObjectMemento schedMem = data->toMemento();

    out << schedMem.toByteArray();

    file.close();
    return true;
}
