/* GTlab - Gas Turbine laboratory
 * Source File: gt_importhandler.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 07.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>

#include "gt_application.h"
#include "gt_abstractimporter.h"
#include "gt_logging.h"

#include "gt_processimporter.h"

#include "gt_importhandler.h"

GtImportHandler::GtImportHandler(QObject* parent) : QObject(parent)
{
    /// Static exporter class registration
    registerClass(GT_METADATA(GtProcessImporter));
}

GtImportHandler*
GtImportHandler::instance()
{
    static GtImportHandler* retval = nullptr;
    if (!retval)
    {
        retval = new GtImportHandler(qApp);
    }
    return retval;
}

bool
GtImportHandler::registerClass(QMetaObject metaObj)
{
    QString classname = metaObj.className();

    //gtDebug() << "register importer = " << classname;

    if (knownClass(classname))
    {
        return false;
    }

    GtImporterMetaData metaData(metaObj);


    if (m_impMeta.contains(metaData.id()))
    {
        return false;
    }

    if (!GtAbstractObjectFactory::registerClass(metaObj))
    {
        return false;
    }

    m_impMeta.insert(metaData.id(), metaData);

    return true;
}

QList<GtImporterMetaData>
GtImportHandler::importerMetaData(const QString& classname)
{
    QList<GtImporterMetaData> retval;

    if (classname.isEmpty())
    {
        return QList<GtImporterMetaData>();
    }

    for (auto const& metaData : qAsConst(m_impMeta))
    {
        foreach (const QString& str, metaData.targets())
        {
            if (str == classname)
            {
                retval << metaData;
            }
        }
    }

    return retval;
}

GtAbstractImporter*
GtImportHandler::newImporter(const QString& classname)
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

    GtAbstractImporter* retval = qobject_cast<GtAbstractImporter*>(obj);

    if (!retval)
    {
        delete obj;
        return nullptr;
    }

    return retval;
}
