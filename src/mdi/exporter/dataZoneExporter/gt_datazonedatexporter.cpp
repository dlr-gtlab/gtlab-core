/* GTlab - Gas Turbine laboratory
 * Source File: gt_datazonedatexporter.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 26.11.2018
 *      Author: Carsten Hollmann (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_datazonedatexporter.h"

#include <QIcon>
#include <QDateTime>

#include "gt_application.h"
#include "gt_project.h"
#include "gt_logging.h"
#include "gt_datazone.h"
#include "gt_datazone0d.h"

GtDataZoneDatExporter::GtDataZoneDatExporter()
{
    setObjectName(tr("to DAT"));
}

QStringList
GtDataZoneDatExporter::targets() const
{
    return QStringList() << GT_CLASSNAME(GtDataZone)
                         << GT_CLASSNAME(GtDataZone0D);
}

QIcon
GtDataZoneDatExporter::icon() const
{
    QIcon icon;
    icon.addFile(QString(":/icons/exportIcon_16.png"),
                 QSize(), QIcon::Normal,
                 QIcon::Off);

    return icon;
}

bool
GtDataZoneDatExporter::doExport(GtObject* data, QFile& file)
{
    if (data == Q_NULLPTR)
    {
        gtError() << tr("Data corrupted!");
        return false;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gtError() << tr("Could not open output file!") << file.fileName();
        return false;
    }

    GtDataZone0D* dataZone0D = qobject_cast<GtDataZone0D*>(data);
    if (dataZone0D != Q_NULLPTR)
    {
        if (!write0Ddata(dataZone0D, file))
        {
            return false;
        }

        file.close();
    }

    GtDataZone* dataZone = qobject_cast<GtDataZone*>(data);
    if (dataZone != Q_NULLPTR)
    {
        if (!writeMultiDimData(dataZone, file))
        {
            return false;
        }

        file.close();
    }

    return true;
}

QString
GtDataZoneDatExporter::extension() const
{
        return QStringLiteral("DAT (*.dat)");
}

QList<GtAbstractExporterPage*>
GtDataZoneDatExporter::pages(GtObject* data)
{
    QList<GtAbstractExporterPage*> retval;

    retval.append(GtAbstractExporter::pages(data));

    return retval;
}

bool
GtDataZoneDatExporter::write0Ddata(GtDataZone0D* data, QFile& file)
{
    if (data == Q_NULLPTR)
    {
        return false;
    }

    QTextStream out(&file);

    if (!writeHeaderLine0D(out, data))
    {
        return false;
    }

    QString paramLine = " ";

    foreach(QString param, data->params())
    {
        if (param != data->params().last())
        {
            paramLine += QString::number(data->value(param)) + "    ";
        }
        else
        {
            paramLine += QString::number(data->value(param));
        }
    }

    paramLine += "\n";

    out << paramLine;

    return true;
}

bool
GtDataZoneDatExporter::writeMultiDimData(GtDataZone* data, QFile& file)
{
    if (data == Q_NULLPTR)
    {
        return false;
    }


    if (data->nDims() == 0)
    {
        return false;
    }

    QTextStream out(&file);

    if (!writeHeaderLine(out, data))
    {
        return false;
    }

    if (data->nDims() == 1)
    {
        return write1Ddata(data, out);
    }
    else if (data->nDims() == 2)
    {
        return write2Ddata(data, out);
    }
    else if (data->nDims() == 3)
    {
        return write3Ddata(data, out);
    }
    else if (data->nDims() == 4)
    {
        return write4Ddata(data, out);
    }
    else
    {
        gtError() << "Dimension of data zone not supported for .dat export!";
    }

    return false;
}

bool
GtDataZoneDatExporter::write1Ddata(GtDataZone* data, QTextStream& out)
{
    if (data == Q_NULLPTR)
    {
        gtDebug() << "Ivalid data zone pointer!";
        return false;
    }

    if (data->nDims() != 1)
    {
        gtDebug() << "Data zone dimension in unequal to 1!";
        return false;
    }

    QString axis1Name = data->axisNames().first();
    QVector<double> ticks1;
    data->axisTicks(axis1Name, ticks1);

    foreach(double tick1, ticks1)
    {
        QString resLine = " " + QString::number(tick1, 'f', 6) + "\t";

        foreach(QString param, data->params())
        {
            resLine += QString::number(data->value1D(
                                           param, tick1), 'f', 6) + "\t";
        }

        resLine += "\n";
        out << resLine;
    }

    return true;
}

bool
GtDataZoneDatExporter::write2Ddata(GtDataZone* data, QTextStream& out)
{
    if (data == Q_NULLPTR)
    {
        gtDebug() << "Ivalid data zone pointer!";
        return false;
    }

    if (data->nDims() != 2)
    {
        gtDebug() << "Data zone dimension in unequal to 2!";
        return false;
    }

    QString axis1Name = data->axisNames().first();
    QVector<double> ticks1;
    data->axisTicks(axis1Name, ticks1);

    QString axis2Name = data->axisNames().last();
    QVector<double> ticks2;
    data->axisTicks(axis1Name, ticks2);

    foreach(double tick1, ticks1)
    {
        foreach(double tick2, ticks2)
        {
            QString resLine = " " + QString::number(tick1, 'f', 6) + "     "
                    + QString::number(tick2, 'f', 6) + "     ";

            foreach(QString param, data->params())
            {
                resLine += QString::number(data->value2D(
                                               param, tick1, tick2), 'f', 6)
                        + "     ";
            }

            resLine += "\n";
            out << resLine;
        }
    }

    return true;
}

bool
GtDataZoneDatExporter::write3Ddata(GtDataZone* data, QTextStream& out)
{
    if (data == Q_NULLPTR)
    {
        gtDebug() << "Ivalid data zone pointer!";
        return false;
    }

    if (data->nDims() != 3)
    {
        gtDebug() << "Data zone dimension in unequal to 3!";
        return false;
    }

    QString axis1Name = data->axisNames().first();
    QVector<double> ticks1;
    data->axisTicks(axis1Name, ticks1);

    QString axis2Name = data->axisNames().at(1);
    QVector<double> ticks2;
    data->axisTicks(axis1Name, ticks2);

    QString axis3Name = data->axisNames().last();
    QVector<double> ticks3;
    data->axisTicks(axis1Name, ticks3);

    foreach(double tick1, ticks1)
    {
        foreach(double tick2, ticks2)
        {
            foreach(double tick3, ticks3)
            {
                QString resLine = " " + QString::number(tick1, 'f', 6) + "     "
                        + QString::number(tick2, 'f', 6) + "     "
                        + QString::number(tick3, 'f', 6) + "     ";

                foreach(QString param, data->params())
                {
                    resLine += QString::number(
                                data->value3D(
                                    param, tick1, tick2, tick3), 'f', 6)
                            + "     ";
                }

                resLine += "\n";
                out << resLine;
            }
        }
    }

    return true;
}

bool
GtDataZoneDatExporter::write4Ddata(GtDataZone* data, QTextStream& out)
{
    if (data == Q_NULLPTR)
    {
        gtDebug() << "Ivalid data zone pointer!";
        return false;
    }

    if (data->nDims() != 4)
    {
        gtDebug() << "Data zone dimension in unequal to 4!";
        return false;
    }

    QString axis1Name = data->axisNames().first();
    QVector<double> ticks1;
    data->axisTicks(axis1Name, ticks1);

    QString axis2Name = data->axisNames().at(1);
    QVector<double> ticks2;
    data->axisTicks(axis1Name, ticks2);

    QString axis3Name = data->axisNames().at(2);
    QVector<double> ticks3;
    data->axisTicks(axis1Name, ticks3);

    QString axis4Name = data->axisNames().last();
    QVector<double> ticks4;
    data->axisTicks(axis1Name, ticks4);

    foreach(double tick1, ticks1)
    {
        foreach(double tick2, ticks2)
        {
            foreach(double tick3, ticks3)
            {
                foreach(double tick4, ticks4)
                {
                    QString resLine = " "
                            + QString::number(tick1, 'f', 6) + "     "
                            + QString::number(tick2, 'f', 6) + "     "
                            + QString::number(tick3, 'f', 6) + "     "
                            + QString::number(tick4, 'f', 6) + "     ";

                    foreach(QString param, data->params())
                    {
                        resLine += QString::number(
                                    data->value4D(param, tick1, tick2,
                                                  tick3, tick4) , 'f', 6)
                                + "     ";
                    }

                    resLine += "\n";
                    out << resLine;
                }
            }
        }
    }

    return true;
}

bool
GtDataZoneDatExporter::writeHeaderLine(QTextStream& out, GtDataZone* dataZone)
{
    if (dataZone == Q_NULLPTR)
    {
        return false;
    }

    if (dataZone->nDims() == 0)
    {
        return false;
    }

    QStringList axisNames = dataZone->axisNames();
    int nLines = dataZone->axisTickStrings(axisNames.first()).size();

    axisNames.removeAll(axisNames.first());

    foreach(QString axisName, axisNames)
    {
        nLines *= dataZone->axisTickStrings(axisName).size();
    }

    QString title = "TITLE = \"GTlab DataZone Export\"\n";

    QString variables;

    variables += "VARIABLES = ";

    foreach(QString axis, dataZone->axisNames())
    {
        variables += "\"" + axis + "\" ";
    }

    foreach(QString param, dataZone->params())
    {
        variables += "\"" + param + "\" ";
    }

    variables += "\n";

    QString zone = "ZONE T = \"" + dataZone->objectName() + "\"\n";

    QString zoneType = "ZONETYPE = ORDERED I = " +QString::number(nLines) +
            " J = 1 K = 1 DATAPACKING = POINT \n";

    out << title;
    out << variables;
    out << zone;
    out << zoneType;

    return true;
}

bool
GtDataZoneDatExporter::writeHeaderLine0D(QTextStream& out,
                                         GtDataZone0D* dataZone)
{
    if (dataZone == Q_NULLPTR)
    {
        return false;
    }

    if (dataZone->nDims() != 0)
    {
        return false;
    }

    QString title = "TITLE = \"GTlab DataZone Export\"\n";

    QString variables;

    variables += "VARIABLES = ";
    foreach(QString param, dataZone->params())
    {
        variables += "\"" + param + "\" ";
    }
    variables += "\n";

    QString zone = "ZONE T = \"" + dataZone->objectName() + "\"\n";

    QString zoneType = "ZONETYPE = ORDERED I = 1 J = 1 K = 1 "
                       "DATAPACKING = POINT \n";

    out << title;
    out << variables;
    out << zone;
    out << zoneType;

    return true;
}