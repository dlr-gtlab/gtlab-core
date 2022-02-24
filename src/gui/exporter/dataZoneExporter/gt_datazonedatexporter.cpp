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
#include "gt_externalizedobjecthelper.h"

namespace
{
    auto concatValues = [](const QString& valueList, const QString& value)
    {
        return  valueList + "\"" + value + "\" ";
    };

    QString listToExportString(const QStringList& list)
    {
        return std::accumulate(std::begin(list), std::end(list), QString(""), concatValues);
    }
}

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
GtDataZoneDatExporter::doExport(GtObject* object, QFile& file)
{
    // fetch datazone for the duration of this function call
    GtExternalizedObjectHelper<GtAbstractDataZone> data
            (object, GtExternalizedObject::Discard);

    if (!data.isValid())
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
        return true;
    }

    GtDataZone* dataZone = qobject_cast<GtDataZone*>(data);
    if (dataZone != Q_NULLPTR)
    {
        if (!writeMultiDimData(dataZone, file))
        {
            return false;
        }

        file.close();
        return true;
    }

    return false;
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
GtDataZoneDatExporter::write0Ddata(const GtDataZone0D* data, QFile& file)
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

    for (const QString& param : data->params())
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
GtDataZoneDatExporter::writeMultiDimData(const GtDataZone* data, QFile& file)
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

    switch (data->nDims())
    {
        case 1:
            return write1Ddata(data, out);
        case 2:
            return write2Ddata(data, out);
        case 3:
            return write3Ddata(data, out);
        case 4:
            return write4Ddata(data, out);
        default:
            gtError() << "Dimension of data zone not supported "
                         "for .dat export!";
    }

    return false;
}

bool
GtDataZoneDatExporter::write1Ddata(const GtDataZone* data, QTextStream& out)
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

    for (const double& tick1 : ticks1)
    {
        QString resLine = " " + QString::number(tick1, 'f', 6) + "\t";

        for (const QString& param : data->params())
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
GtDataZoneDatExporter::write2Ddata(const GtDataZone* data, QTextStream& out)
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

    const auto axNames = data->axisNames();

    QString axis1Name{"Axis 1"};
    if (!axNames.isEmpty()) axis1Name = axNames.constFirst();

    QVector<double> ticks1;
    data->axisTicks(axis1Name, ticks1);

    QString axis2Name{"Axis 2"};
    if (!axNames.isEmpty()) axis2Name = axNames.constLast();

    QVector<double> ticks2;
    data->axisTicks(axis2Name, ticks2);

    foreach (const double& tick1, ticks1)
    {
        foreach (const double& tick2, ticks2)
        {
            QString resLine = " " + QString::number(tick1, 'f', 6) + "     "
                    + QString::number(tick2, 'f', 6) + "     ";

            for (const QString& param : data->params())
            {
                resLine += QString::number(data->value2D( param, tick1, tick2),
                                           'f', 6) + "     ";
            }

            resLine += "\n";
            out << resLine;
        }
    }

    return true;
}

bool
GtDataZoneDatExporter::write3Ddata(const GtDataZone* data, QTextStream& out)
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
    data->axisTicks(axis2Name, ticks2);

    QString axis3Name = data->axisNames().last();
    QVector<double> ticks3;
    data->axisTicks(axis3Name, ticks3);

    for (const double& tick1 : ticks1)
    {
        for (const double& tick2 : ticks2)
        {
            for (const double& tick3 : ticks3)
            {
                QString resLine = " " + QString::number(tick1, 'f', 6) + "     "
                        + QString::number(tick2, 'f', 6) + "     "
                        + QString::number(tick3, 'f', 6) + "     ";

                for (const QString& param : data->params())
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
GtDataZoneDatExporter::write4Ddata(const GtDataZone* data, QTextStream& out)
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
    data->axisTicks(axis2Name, ticks2);

    QString axis3Name = data->axisNames().at(2);
    QVector<double> ticks3;
    data->axisTicks(axis3Name, ticks3);

    QString axis4Name = data->axisNames().last();
    QVector<double> ticks4;
    data->axisTicks(axis4Name, ticks4);

    foreach (const double& tick1, ticks1)
    {
        foreach (const double& tick2, ticks2)
        {
            foreach (const double& tick3, ticks3)
            {
                foreach (const double& tick4, ticks4)
                {
                    QString resLine = " "
                            + QString::number(tick1, 'f', 6) + "     "
                            + QString::number(tick2, 'f', 6) + "     "
                            + QString::number(tick3, 'f', 6) + "     "
                            + QString::number(tick4, 'f', 6) + "     ";

                    foreach (const QString& param, data->params())
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
GtDataZoneDatExporter::writeHeaderLine(QTextStream& out, const GtDataZone* dataZone)
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

    foreach (const auto& axisName, axisNames)
    {
        nLines *= dataZone->axisTickStrings(axisName).size();
    }

    QString title = "TITLE = \"GTlab DataZone Export\"\n";

    QString variables ("VARIABLES = ");
    variables += listToExportString(dataZone->axisNames());
    variables += listToExportString(dataZone->params());
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
                                         const GtDataZone0D* dataZone)
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

    QString variables{"VARIABLES = "};
    variables += listToExportString(dataZone->params());
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
