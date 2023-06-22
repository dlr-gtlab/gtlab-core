/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_exporthandler.cpp
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_exporthandler.h"

#include <QCoreApplication>

#include "gt_abstractexporter.h"
#include "gt_algorithms.h"

#include "gt_processexporter.h"

GtExportHandler::GtExportHandler(QObject* parent) : QObject(parent)
{
    /// Static exporter class registration
    GtExportHandler::registerClass(GT_METADATA(GtProcessExporter));
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

    gt::for_each_key(m_expMeta, [&](const QString& e)
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
