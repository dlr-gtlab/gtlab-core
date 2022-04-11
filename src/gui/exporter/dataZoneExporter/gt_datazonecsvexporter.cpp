/* GTlab - Gas Turbine laboratory
 * Source File: gt_datazonecsvexporter.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 06.11.2018
 *      Author: Carsten Hollmann (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_datazonecsvexporter.h"

#include <QIcon>
#include <QDateTime>

#include "gt_application.h"
#include "gt_project.h"
#include "gt_logging.h"
#include "gt_datazone.h"
#include "gt_datazone0d.h"
#include "gt_externalizedobjecthelper.h"

GtDataZoneCsvExporter::GtDataZoneCsvExporter()
{
    setObjectName(tr("to CSV"));
}

QStringList
GtDataZoneCsvExporter::targets() const
{
    return QStringList() << GT_CLASSNAME(GtDataZone)
                         << GT_CLASSNAME(GtDataZone0D);
}

QIcon
GtDataZoneCsvExporter::icon() const
{
    QIcon icon;
    icon.addFile(QString(":/icons/exportIcon_16.png"),
                 QSize(), QIcon::Normal,
                 QIcon::Off);

    return icon;
}

bool
GtDataZoneCsvExporter::doExport(GtObject* object, QFile& file)
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

    GtDataZone0D* dataZone0D = qobject_cast<GtDataZone0D*>(data.get());
    if (dataZone0D)
    {
        if (!write0Ddata(dataZone0D, file))
        {
            return false;
        }

        file.close();
        return true;
    }

    GtDataZone* dataZone = qobject_cast<GtDataZone*>(data.get());
    if (dataZone)
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
GtDataZoneCsvExporter::extension() const
{
    return QStringLiteral("CSV (*.csv)");
}

QList<GtAbstractExporterPage*>
GtDataZoneCsvExporter::pages(GtObject* data)
{
    QList<GtAbstractExporterPage*> retval;

    retval.append(GtAbstractExporter::pages(data));

    return retval;
}

bool
GtDataZoneCsvExporter::write0Ddata(GtDataZone0D* data, QFile& file)
{
    if (!data)
    {
        return false;
    }

    QTextStream out(&file);

    if (!writeHeaderLine(out, data))
    {
        return false;
    }

    for (const QString& param : data->params())
    {
        out << QStringLiteral("\"")
            << param << QStringLiteral("\";")
            << QStringLiteral("\"")
            << data->unit(param) << QStringLiteral("\";")
            << QString::number(data->value(param)) << QStringLiteral(";")
            << QStringLiteral("\n");
    }

    return true;
}

bool
GtDataZoneCsvExporter::writeMultiDimData(GtDataZone* data, QFile& file)
{
    if (!data)
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
                         "for .csv export!";
    }

    return false;
}

bool
GtDataZoneCsvExporter::write1Ddata(GtDataZone* data, QTextStream& out)
{
    if (!data)
    {
        gtDebug() << "Ivalid data zone pointer!";
        return false;
    }

    if (data->nDims() != 1)
    {
        gtDebug() << "Data zone dimension in unequal to 1!";
        return false;
    }

    auto axisNames = data->axisNames();
    QString axisName = axisNames.first();
    QVector<double> ticks;
    data->axisTicks(axisName, ticks);

    out << QStringLiteral("\"") << axisName << QStringLiteral("\";");
    out << QStringLiteral("\"") << "-" << QStringLiteral("\";");

    for (const double& tick : qAsConst(ticks))
    {
        out << QString::number(tick) << QStringLiteral(";");
    }

    out << QStringLiteral("\n");

    for (const QString& param : data->params())
    {
        out << QStringLiteral("\"")
            << param << QStringLiteral("\";");

        out <<  QStringLiteral("\"")
             << data->unit(param) << QStringLiteral("\";");

        for (const double& tick : qAsConst(ticks))
        {
            out << QString::number(data->value1D(param, tick))
                << QStringLiteral(";");
        }

        out << QStringLiteral("\n");
    }

    return true;
}

bool
GtDataZoneCsvExporter::write2Ddata(GtDataZone* data, QTextStream& out)
{
    if (!data)
    {
        gtDebug() << "Ivalid data zone pointer!";
        return false;
    }

    if (data->nDims() != 2)
    {
        gtDebug() << "Data zone dimension in unequal to 2!";
        return false;
    }

    auto axisNames = data->axisNames();
    QString axisName1 = axisNames.first();
    QString axisName2 = axisNames.last();

    QVector<double> axis1Ticks = data->axisTicks(axisName1);
    QVector<double> axis2Ticks = data->axisTicks(axisName2);

    QString axis1Line;
    QString axis2Line;

    axis1Line += QStringLiteral("\"") + axisName1 + QStringLiteral("\";");
    axis1Line += QStringLiteral("\"") + "-" + QStringLiteral("\";");

    axis2Line += QStringLiteral("\"") + axisName2 + QStringLiteral("\";");
    axis2Line += QStringLiteral("\"") + "-" + QStringLiteral("\";");

    for (int i = 0; i < axis1Ticks.size(); i++)
    {
        for (int j = 0; j < axis2Ticks.size(); j++)
        {
            axis1Line += QString::number(axis1Ticks.at(i))
                    + QStringLiteral(";");

            axis2Line += QString::number(axis2Ticks.at(j))
                    + QStringLiteral(";");
        }
    }

    out << axis1Line << QStringLiteral("\n");
    out << axis2Line << QStringLiteral("\n");

    for (const QString& param : data->params())
    {
        out << QStringLiteral("\"")
            << param << QStringLiteral("\";");

        out <<  QStringLiteral("\"")
             << data->unit(param) << QStringLiteral("\";");

        for (const double& tick1 : qAsConst(axis1Ticks))
        {
            for (const double& tick2 : qAsConst(axis2Ticks))
            {
                out << QString::number(data->value2D(param, tick1, tick2))
                    << QStringLiteral(";");
            }
        }

        out << QStringLiteral("\n");
    }

    return true;
}

bool
GtDataZoneCsvExporter::write3Ddata(GtDataZone* data, QTextStream& out)
{
    if (!data)
    {
        gtDebug() << "Ivalid data zone pointer!";
        return false;
    }

    if (data->nDims() != 3)
    {
        gtDebug() << "Data zone dimension in unequal to 3!";
        return false;
    }

    auto axisNames = data->axisNames();
    QString axisName1 = axisNames.first();
    QString axisName2 = axisNames.at(1);
    QString axisName3 = axisNames.last();

    QVector<double> axis1Ticks = data->axisTicks(axisName1);
    QVector<double> axis2Ticks = data->axisTicks(axisName2);
    QVector<double> axis3Ticks = data->axisTicks(axisName3);

    QString axis1Line;
    QString axis2Line;
    QString axis3Line;

    axis1Line += QStringLiteral("\"") + axisName1 + QStringLiteral("\";");
    axis1Line += QStringLiteral("\"") + "-" + QStringLiteral("\";");

    axis2Line += QStringLiteral("\"") + axisName2 + QStringLiteral("\";");
    axis2Line += QStringLiteral("\"") + "-" + QStringLiteral("\";");

    axis3Line += QStringLiteral("\"") + axisName3 + QStringLiteral("\";");
    axis3Line += QStringLiteral("\"") + "-" + QStringLiteral("\";");

    for (int i = 0; i < axis1Ticks.size(); i++)
    {
        for (int j = 0; j < axis2Ticks.size(); j++)
        {
            for (int k = 0; k < axis3Ticks.size(); k++)
            {
                axis1Line += QString::number(axis1Ticks.at(i))
                        + QStringLiteral(";");

                axis2Line += QString::number(axis2Ticks.at(j))
                        + QStringLiteral(";");

                axis3Line += QString::number(axis3Ticks.at(k))
                        + QStringLiteral(";");
            }
        }
    }

    out << axis1Line << QStringLiteral("\n");
    out << axis2Line << QStringLiteral("\n");
    out << axis3Line << QStringLiteral("\n");

    for (const QString& param : data->params())
    {
        out << QStringLiteral("\"")
            << param << QStringLiteral("\";");

        out <<  QStringLiteral("\"")
             << data->unit(param) << QStringLiteral("\";");

        for (const double& tick1 : qAsConst(axis1Ticks))
        {
            for (const double& tick2 : qAsConst(axis2Ticks))
            {
                for (const double& tick3 : qAsConst(axis3Ticks))
                {
                    out << QString::number(data->value3D(param,
                                                         tick1, tick2, tick3))
                        << QStringLiteral(";");
                }
            }
        }

        out << QStringLiteral("\n");
    }

    return true;
}

bool
GtDataZoneCsvExporter::write4Ddata(GtDataZone* data, QTextStream &out)
{
    if (!data)
    {
        gtDebug() << "Ivalid data zone pointer!";
        return false;
    }

    if (data->nDims() != 4)
    {
        gtDebug() << "Data zone dimension in unequal to 1!";
        return false;
    }

    const auto axisNames = data->axisNames();
    QString axisName1 = axisNames.first();
    QString axisName2 = axisNames.at(1);
    QString axisName3 = axisNames.at(2);
    QString axisName4 = axisNames.last();

    QVector<double> axis1Ticks = data->axisTicks(axisName1);
    QVector<double> axis2Ticks = data->axisTicks(axisName2);
    QVector<double> axis3Ticks = data->axisTicks(axisName3);
    QVector<double> axis4Ticks = data->axisTicks(axisName4);

    QString axis1Line;
    QString axis2Line;
    QString axis3Line;
    QString axis4Line;

    axis1Line += QStringLiteral("\"") + axisName1 + QStringLiteral("\";");
    axis1Line += QStringLiteral("\"") + "-" + QStringLiteral("\";");

    axis2Line += QStringLiteral("\"") + axisName2 + QStringLiteral("\";");
    axis2Line += QStringLiteral("\"") + "-" + QStringLiteral("\";");

    axis3Line += QStringLiteral("\"") + axisName3 + QStringLiteral("\";");
    axis3Line += QStringLiteral("\"") + "-" + QStringLiteral("\";");

    axis4Line += QStringLiteral("\"") + axisName4 + QStringLiteral("\";");
    axis4Line += QStringLiteral("\"") + "-" + QStringLiteral("\";");

    for (int i = 0; i < axis1Ticks.size(); i++)
    {
        for (int j = 0; j < axis2Ticks.size(); j++)
        {
            for (int k = 0; k < axis3Ticks.size(); k++)
            {
                for (int l = 0; l < axis4Ticks.size(); l++)
                {
                    axis1Line += QString::number(axis1Ticks.at(i))
                            + QStringLiteral(";");

                    axis2Line += QString::number(axis2Ticks.at(j))
                            + QStringLiteral(";");

                    axis3Line += QString::number(axis3Ticks.at(k))
                            + QStringLiteral(";");

                    axis4Line += QString::number(axis4Ticks.at(l))
                            + QStringLiteral(";");
                }
            }
        }
    }

    out << axis1Line << QStringLiteral("\n");
    out << axis2Line << QStringLiteral("\n");
    out << axis3Line << QStringLiteral("\n");
    out << axis4Line << QStringLiteral("\n");

    for (const QString& param : data->params())
    {
        out << QStringLiteral("\"")
            << param << QStringLiteral("\";");

        out <<  QStringLiteral("\"")
             << data->unit(param) << QStringLiteral("\";");

        for (const double& tick1 : qAsConst(axis1Ticks))
        {
            for (const double& tick2 : qAsConst(axis2Ticks))
            {
                for (const double& tick3 : qAsConst(axis3Ticks))
                {
                    for (const double& tick4 : qAsConst(axis4Ticks))
                    {
                        out << QString::number(data->value4D(param, tick1,
                                                             tick2, tick3,
                                                             tick4))
                            << QStringLiteral(";");
                    }
                }
            }
        }

        out << QStringLiteral("\n");
    }

    return true;
}

bool
GtDataZoneCsvExporter::writeHeaderLine(QTextStream& out,
                                       GtAbstractDataZone* dataZone)
{
    if (!dataZone)
    {
        return false;
    }

    int nParams = dataZone->params().size();

    QDateTime now = QDateTime::currentDateTime();
    QString nowStr = now.toString("yyyyMMdd-hh:mm:ss");

    writeHeaderLineHelper(out, "DataZone", dataZone->objectName(),
                          nParams);
    writeHeaderLineHelper(out, "UUID", dataZone->uuid(),
                          nParams);
    writeHeaderLineHelper(out, "Project", gtApp->currentProject()->objectName(),
                          nParams);
    writeHeaderLineHelper(out, "Export Time", nowStr,
                          nParams);

    out << QStringLiteral("\n");

    return true;
}

void
GtDataZoneCsvExporter::writeHeaderLineHelper(
        QTextStream& out, const QString &p, const QString& v, int nParams)
{
    out << QStringLiteral("\"") << p << QStringLiteral("\";\"") << v <<
           QStringLiteral("\";");

    for (int i = 1; i < nParams; ++i)
    {
        out << QStringLiteral(";") << QStringLiteral("");
    }

    out << QStringLiteral("\n");
}

