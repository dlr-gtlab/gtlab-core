/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractexporter.cpp
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
