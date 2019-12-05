/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractimporter.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 07.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QStringList>

#include "gt_selectfiletoimportpage.h"

#include "gt_abstractimporter.h"

GtAbstractImporter::GtAbstractImporter()
{

}

QStringList
GtAbstractImporter::extensions() const
{
    return QStringList();
}

QList<GtAbstractImporterPage*>
GtAbstractImporter::pages(GtObject* data)
{
    QList<GtAbstractImporterPage*> retval;

    retval << new GtSelectFileToImportPage(data, this);

    return retval;
}
