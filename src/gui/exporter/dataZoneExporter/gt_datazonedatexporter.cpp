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
#include <QFile>
#include <QTextStream>

#include "gt_logging.h"
#include "gt_datazone.h"
#include "gt_datazone0d.h"
#include "gt_icons.h"

namespace
{
    auto concatValues = [](const QString& valueList, const QString& value)
    {
        return  valueList + "\"" + value + "\" ";
    };

    QString listToExportString(const QStringList& list)
    {
        return std::accumulate(std::begin(list), std::end(list), QString{},
                               concatValues);
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
    return GtGUI::Icon::export16();
}

bool
GtDataZoneDatExporter::doExport(GtObject* object, QFile& file)
{
    // fetch datazone for the duration of this function call
    auto* adz = qobject_cast<GtAbstractDataZone*>(object);

    if (adz == nullptr)
    {
        gtError() << tr("Data corrupted!");
        return false;
    }

    auto data = adz->fetchData();

    if (!data.isValid())
    {
        gtError() << tr("Invalid data!");
        return false;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gtError() << tr("Could not open output file!") << file.fileName();
        return false;
    }

    if (auto* dataZone0D = qobject_cast<GtDataZone0D*>(adz))
    {
        return write0Ddata(*dataZone0D, file);
    }

    if (auto* dataZone = qobject_cast<GtDataZone*>(adz))
    {
        return writeMultiDimData(*dataZone, file);
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
GtDataZoneDatExporter::write0Ddata(GtDataZone0D& dataZone, QFile& file)
{
    auto data{dataZone.fetchData()};

    if (!data.isValid())
    {
        return false;
    }

    QTextStream out(&file);

    if (!writeHeaderLine0D(dataZone, data, out))
    {
        return false;
    }

    QString paramLine{ QStringLiteral(" ") };

    for (const QString& param : data.params())
    {
        if (param != data.params().last())
        {
            paramLine += QString::number(data.value(param))
                      +  QStringLiteral("    ");
        }
        else
        {
            paramLine += QString::number(data.value(param));
        }
    }

    paramLine += QStringLiteral("\n");

    out << paramLine;

    return true;
}

bool
GtDataZoneDatExporter::writeMultiDimData(GtDataZone& dataZone, QFile& file)
{
    auto data{dataZone.fetchData()};

    if (!data.isValid())
    {
        return false;
    }

    if (dataZone.nDims() == 0)
    {
        return false;
    }

    QTextStream out(&file);

    if (!writeHeaderLine(dataZone, data, out))
    {
        return false;
    }

    switch (dataZone.nDims())
    {
        case 1:
            return write1Ddata(dataZone, data, out);
        case 2:
            return write2Ddata(dataZone, data, out);
        case 3:
            return write3Ddata(dataZone, data, out);
        case 4:
            return write4Ddata(dataZone, data, out);
        default:
            gtError() << "Dimension of data zone not supported for .dat export!";
            return false;
    }
}

bool
GtDataZoneDatExporter::write1Ddata(const GtDataZone& dataZone,
                                   const GtDataZone::Data& data,
                                   QTextStream& out)
{
    if (dataZone.nDims() != 1)
    {
        gtDebug() << "Data zone dimension in unequal to 1!";
        return false;
    }

    QString axis1Name = dataZone.axisNames().first();
    QVector<double> ticks1;
    dataZone.axisTicks(axis1Name, ticks1);

    foreach (const double& tick1, ticks1)
    {
        QString resLine{ QStringLiteral(" ") +
                         QString::number(tick1, 'f', 6) +
                         QStringLiteral("\t") };

        for (const QString& param : data.params())
        {
            resLine += QString::number(data.value1D(param, tick1), 'f', 6)
                    +  QStringLiteral("\t");
        }

        resLine += QStringLiteral("\n");
        out << resLine;
    }

    return true;
}

bool
GtDataZoneDatExporter::write2Ddata(const GtDataZone& dataZone,
                                   const GtDataZone::Data& data,
                                   QTextStream& out)
{
    if (dataZone.nDims() != 2)
    {
        gtDebug() << "Data zone dimension in unequal to 2!";
        return false;
    }

    const auto& axNames = dataZone.axisNames();

    QString axis1Name{"Axis 1"};
    if (!axNames.isEmpty()) axis1Name = axNames.constFirst();

    QVector<double> ticks1;
    dataZone.axisTicks(axis1Name, ticks1);

    QString axis2Name{"Axis 2"};
    if (!axNames.isEmpty()) axis2Name = axNames.constLast();

    QVector<double> ticks2;
    dataZone.axisTicks(axis2Name, ticks2);

    foreach (const double& tick1, ticks1)
    {
        foreach (const double& tick2, ticks2)
        {
            QString resLine{ QStringLiteral(" ") +
                             QString::number(tick1, 'f', 6) +
                             QStringLiteral("     ") +
                             QString::number(tick2, 'f', 6) +
                             QStringLiteral("     ") };

            for (const QString& param : data.params())
            {
                resLine += QString::number(data.value2D(param, tick1, tick2),
                                           'f', 6) + QStringLiteral("     ");
            }

            resLine += QStringLiteral("\n");
            out << resLine;
        }
    }

    return true;
}

bool
GtDataZoneDatExporter::write3Ddata(const GtDataZone& dataZone,
                                   const GtDataZone::Data& data,
                                   QTextStream& out)
{
    if (dataZone.nDims() != 3)
    {
        gtDebug() << "Data zone dimension in unequal to 3!";
        return false;
    }

    QString axis1Name = dataZone.axisNames().first();
    QVector<double> ticks1;
    dataZone.axisTicks(axis1Name, ticks1);

    QString axis2Name = dataZone.axisNames().at(1);
    QVector<double> ticks2;
    dataZone.axisTicks(axis2Name, ticks2);

    QString axis3Name = dataZone.axisNames().last();
    QVector<double> ticks3;
    dataZone.axisTicks(axis3Name, ticks3);

    foreach (const double& tick1, ticks1)
    {
        foreach (const double& tick2, ticks2)
        {
            foreach (const double& tick3, ticks3)
            {
                QString resLine{ QStringLiteral(" ") +
                                 QString::number(tick1, 'f', 6) +
                                 QStringLiteral("     ") +
                                 QString::number(tick2, 'f', 6) +
                                 QStringLiteral("     ") +
                                 QString::number(tick3, 'f', 6) +
                                 QStringLiteral("     ") };

                for (const QString& param : data.params())
                {
                    resLine += QString::number(data.value3D(
                                    param, tick1, tick2, tick3), 'f', 6)
                            + QStringLiteral("     ");
                }

                resLine += QStringLiteral("\n");
                out << resLine;
            }
        }
    }

    return true;
}

bool
GtDataZoneDatExporter::write4Ddata(const GtDataZone& dataZone,
                                   const GtDataZone::Data& data,
                                   QTextStream& out)
{
    if (dataZone.nDims() != 4)
    {
        gtDebug() << "Data zone dimension in unequal to 4!";
        return false;
    }

    QString axis1Name = dataZone.axisNames().first();
    QVector<double> ticks1;
    dataZone.axisTicks(axis1Name, ticks1);

    QString axis2Name = dataZone.axisNames().at(1);
    QVector<double> ticks2;
    dataZone.axisTicks(axis2Name, ticks2);

    QString axis3Name = dataZone.axisNames().at(2);
    QVector<double> ticks3;
    dataZone.axisTicks(axis3Name, ticks3);

    QString axis4Name = dataZone.axisNames().last();
    QVector<double> ticks4;
    dataZone.axisTicks(axis4Name, ticks4);

    foreach (const double& tick1, ticks1)
    {
        foreach (const double& tick2, ticks2)
        {
            foreach (const double& tick3, ticks3)
            {
                foreach (const double& tick4, ticks4)
                {
                    QString resLine{ QStringLiteral(" ") +
                            QString::number(tick1, 'f', 6) +
                            QStringLiteral("     ") +
                            QString::number(tick2, 'f', 6) +
                            QStringLiteral("     ") +
                            QString::number(tick3, 'f', 6) +
                            QStringLiteral("     ") +
                            QString::number(tick4, 'f', 6) +
                            QStringLiteral("     ") };

                    for (const QString& param : data.params())
                    {
                        resLine += QString::number(data.value4D(param, tick1,
                                                                tick2, tick3,
                                                                tick4) , 'f', 6)
                                +  QStringLiteral("     ");
                    }

                    resLine += QStringLiteral("\n");
                    out << resLine;
                }
            }
        }
    }

    return true;
}

bool
GtDataZoneDatExporter::writeHeaderLine(const GtDataZone& dataZone,
                                       const GtDataZone::Data& data,
                                       QTextStream& out)
{
    if (dataZone.nDims() == 0)
    {
        return false;
    }

    QStringList axisNames = dataZone.axisNames();
    int nLines = dataZone.axisTickStrings(axisNames.first()).size();

    axisNames.removeAll(axisNames.first());

    foreach (const auto& axisName, axisNames)
    {
        nLines *= dataZone.axisTickStrings(axisName).size();
    }

    QString title = QStringLiteral("TITLE = \"GTlab DataZone Export\"\n");

    QString variables ("VARIABLES = ");
    variables += listToExportString(dataZone.axisNames());
    variables += listToExportString(data.params());
    variables += "\n";

    QString zone = QStringLiteral("ZONE T = \"") + dataZone.objectName() +
            QStringLiteral("\"\n");

    QString zoneType = QStringLiteral("ZONETYPE = ORDERED I = ") +
            QString::number(nLines) +
            QStringLiteral(" J = 1 K = 1 DATAPACKING = POINT \n");

    out << title;
    out << variables;
    out << zone;
    out << zoneType;

    return true;
}

bool
GtDataZoneDatExporter::writeHeaderLine0D(const GtDataZone0D& dataZone,
                                         const GtDataZone0D::Data& data,
                                         QTextStream& out)
{
    if (dataZone.nDims() != 0)
    {
        return false;
    }

    QString title = QStringLiteral("TITLE = \"GTlab DataZone Export\"\n");

    QString variables{"VARIABLES = "};
    variables += listToExportString(data.params());
    variables += "\n";

    QString zone = QStringLiteral("ZONE T = \"") + dataZone.objectName() +
            QStringLiteral("\"\n");

    QString zoneType = QStringLiteral("ZONETYPE = ORDERED I = 1 J = 1 K = 1 "
                                      "DATAPACKING = POINT \n");

    out << title;
    out << variables;
    out << zone;
    out << zoneType;

    return true;
}
