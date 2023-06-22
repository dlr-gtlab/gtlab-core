/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processexporter.cpp
 *
 *  Created on: 04.05.2020
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */

#include <QIcon>

#include "gt_logging.h"
#include "gt_processcomponent.h"
#include "gt_objectmemento.h"
#include "gt_calculatorfactory.h"
#include "gt_taskfactory.h"
#include "gt_icons.h"

#include "gt_processexporter.h"

GtProcessExporter::GtProcessExporter()
{
    setObjectName(tr("Process Exporter"));
}

QStringList
GtProcessExporter::targets() const
{
    QStringList retVal;
    foreach (GtCalculatorData calcData,
             gtCalculatorFactory->calculatorDataList())
    {
        QString tmpStr = calcData->metaData().className();
        retVal.append(tmpStr);
    }

    foreach (GtTaskData taskData,
             gtTaskFactory->taskDataList())
    {
        QString tmpStr = taskData->metaData().className();
        retVal.append(tmpStr);
    }

    return retVal;
}

QIcon
GtProcessExporter::icon() const
{
    return gt::gui::icon::export_();
}

bool
GtProcessExporter::doExport(GtObject* data, QFile& file)
{
    if (!data)
    {
        gtError() << tr("Data corrupted!");
        return false;
    }

    GtProcessComponent* element = qobject_cast<GtProcessComponent*>(data);

    if (!element)
    {
        gtError() << tr("Process Component corrupted!");
        return false;
    }

    gtInfo() << tr("exporting ") << data->objectName() << " to "
             << file.fileName();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        gtError() << tr("Could not open file!") << file.fileName();
        return false;
    }

    QTextStream out(&file);

    QDomDocument doc;
    doc.setContent(data->toMemento(false).toByteArray());

    out << doc.toString();

    file.close();

    gtInfo() << "Process export finished succesfull";

    return true;
}

QString
GtProcessExporter::extension() const
{
    return QStringLiteral("XML (*.xml)");
}

