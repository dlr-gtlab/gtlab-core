/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractexporter.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_selectfiletoexportpage.h"

#include "gt_abstractexporter.h"

GtAbstractExporter::GtAbstractExporter()
{

}

QString
GtAbstractExporter::extension() const
{
    return QString();
}

QList<GtAbstractExporterPage*>
GtAbstractExporter::pages(GtObject* data)
{
    QList<GtAbstractExporterPage*> retval;

    retval << new GtSelectFileToExportPage(data, this);

    return retval;
}
