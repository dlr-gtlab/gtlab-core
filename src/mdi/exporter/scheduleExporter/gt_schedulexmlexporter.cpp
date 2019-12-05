/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2019 by DLR
 *
 *  Created on: 01.08.2019
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 2579
 */
#include "gt_schedulexmlexporter.h"

#include <QIcon>
#include <QFile>

#include "gt_logging.h"
#include "gt_controlschedule.h"

GtpScheduleXmlExporter::GtpScheduleXmlExporter()
{
    setObjectName(tr("XML Exporter"));
}

QStringList
GtpScheduleXmlExporter::targets() const
{
    return QStringList() << GT_CLASSNAME(GtControlSchedule);
}

QIcon
GtpScheduleXmlExporter::icon() const
{
    QIcon icon;
    icon.addFile(QString(":/icons/exportIcon_16.png"),
                 QSize(), QIcon::Normal,
                 QIcon::Off);

    return icon;
}

bool
GtpScheduleXmlExporter::doExport(GtObject* data, QFile& file)
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
GtpScheduleXmlExporter::extension() const
{
    return QStringLiteral("XML (*.xml)");
}

QList<GtAbstractExporterPage*>
GtpScheduleXmlExporter::pages(GtObject* data)
{
    QList<GtAbstractExporterPage*> retval;

    retval.append(GtAbstractExporter::pages(data));

    return retval;
}

bool
GtpScheduleXmlExporter::writeData(GtControlSchedule* data, QFile& file)
{
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gtError() << tr("Could not open output file!") << file.fileName();
        return false;
    }

    if (data == Q_NULLPTR)
    {
        file.close();
        return false;
    }

    QString tabName = data->objectName();

    QTextStream out(&file);

    QDomDocument xmlMap;

    QDomElement root = xmlMap.createElement("GTlabControlSchedule");

    QDomElement tabElem = data->toDomElement(xmlMap);

    root.appendChild(tabElem);
    xmlMap.appendChild(root);
    xmlMap.save(out, 2);

    file.close();

    return true;
}
