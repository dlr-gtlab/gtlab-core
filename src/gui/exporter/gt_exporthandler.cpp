/* GTlab - Gas Turbine laboratory
 * Source File: gt_exporthandler.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_exporthandler.h"

#include <QCoreApplication>

#include "gt_application.h"
#include "gt_abstractexporter.h"
#include "gt_logging.h"
#include "gt_algorithms.h"

#include "gt_processexporter.h"
#include "gt_datazonecsvexporter.h"
#include "gt_datazonedatexporter.h"

GtExportHandler::GtExportHandler(QObject* parent) : QObject(parent)
{
    /// Static exporter class registration
    GtExportHandler::registerClass(GT_METADATA(GtProcessExporter));
    GtExportHandler::registerClass(GT_METADATA(GtDataZoneCsvExporter));
    GtExportHandler::registerClass(GT_METADATA(GtDataZoneDatExporter));
}

GtExportHandler*
GtExportHandler::instance()
{
    static GtExportHandler* retval = nullptr;
    if (!retval)
    {
        retval = new GtExportHandler(qApp);
    }
    return retval;
}

bool
GtExportHandler::registerClass(QMetaObject metaObj)
{
    QString classname = metaObj.className();

    //gtDebug() << "register exporter = " << classname;

    if (knownClass(classname))
    {
        return false;
    }

    GtExporterMetaData metaData(metaObj);


    if (m_expMeta.contains(metaData.id()))
    {
        return false;
    }

    if (!GtAbstractObjectFactory::registerClass(metaObj))
    {
        return false;
    }

    m_expMeta.insert(metaData.id(), metaData);

    return true;
}

QList<GtExporterMetaData>
GtExportHandler::exporterMetaData(const QString& classname)
{
    QList<GtExporterMetaData> retval;

    if (classname.isEmpty())
    {
        return QList<GtExporterMetaData>();
    }

    for_each_key(m_expMeta, [&](const QString& e)
    {
        GtExporterMetaData metaData = m_expMeta.value(e);

        foreach (const QString& str, metaData.targets())
        {
            if (str == classname)
            {
                retval << metaData;
            }
        }
    });

    return retval;
}

GtAbstractExporter*
GtExportHandler::newExporter(const QString& classname)
{
    if (!knownClass(classname))
    {
        return nullptr;
    }

    GtObject* obj = newObject(classname);

    if (!obj)
    {
        return nullptr;
    }

    GtAbstractExporter* retval = qobject_cast<GtAbstractExporter*>(obj);

    if (!retval)
    {
        delete obj;
        return nullptr;
    }

    return retval;
}