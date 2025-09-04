/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 19.06.2024
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QFile>
#include <QDomDocument>

#include "gt_logging.h"
#include "gt_coreapplication.h"
#include "gt_project.h"
#include "gt_objectmemento.h"
#include "gt_xmlutilities.h"

#include "gt_exporttomementocalculator.h"

GtExportToMementoCalculator::GtExportToMementoCalculator() :
    m_fileMode("fileMode", tr("File Path Mode"), tr("File Path Mode")),
    m_relativeFileMode(tr("Relative to Working Directory"),
                       tr("Relative to Working Directory")),
    m_relativeToProjectFileMode(tr("Relative to Project Directory"),
                                tr("Relative to Project Directory")),
    m_absoluteFileMode(tr("Absolute"), tr("Absolut")),
    m_absoluteFilePath("absPath" ,tr("File Path"), tr("File Path"),
                       QStringList() << "XML (*.xml)", "memento.xml"),
    m_relativeFilePath("relPath",tr("File Path"), tr("File Path"),
                       "memento.xml"),
    m_targetObject("obj", "Object", "Target object.", this,
                   QStringList() << GT_CLASSNAME(GtObject), true)
{
    setObjectName("Export Object to Memento");

    registerProperty(m_targetObject);

    m_absoluteFileMode.registerSubProperty(m_absoluteFilePath);
    m_fileMode.registerSubProperty(m_absoluteFileMode);

    m_relativeFileMode.registerSubProperty(m_relativeFilePath);
    m_relativeToProjectFileMode.registerSubProperty(m_relativeFilePath);
    m_fileMode.registerSubProperty(m_relativeFileMode);
    m_fileMode.registerSubProperty(m_relativeToProjectFileMode);

    registerProperty(m_fileMode);

    // hide label
    hideLabelProperty(true);
}

bool
GtExportToMementoCalculator::run()
{
    GtObject* obj = data<GtObject*>(m_targetObject);

    if (!obj)
    {
        gtError() << QObject::tr("GtExportToMementoCalculator: no object selected!");
        return false;
    }

    QString path;

    if (m_fileMode.get() == m_absoluteFileMode.get())
    {
        // absolute mode
        gtDebug() << "absolute mode";
        path = m_absoluteFilePath.get();
    }
    else
    {
        // relative mode
        if (m_fileMode.get() == m_relativeFileMode.get())
        {
            // to working directory
            gtDebug() << "relative to working directory mode";
            path = m_relativeFilePath;
        }
        else
        {
            // to project directory
            gtDebug() << "relative to project directory mode";
            if (gtApp->currentProject())
            {
                path = gtApp->currentProject()->path() + QDir::separator() +
                       m_relativeFilePath;
            }
        }
    }

    if (path.isEmpty())
    {
        gtError() << QObject::tr("GtExportToMementoCalculator: Path is empty");
        return false;
    }

    QFileInfo fileInfo(path);
    QString directory = fileInfo.absolutePath();

    QDir dir;
    if (!dir.mkpath(directory))
    {
        gtError() << QObject::tr("GtExportToMementoCalculator: Could not create directory '%1'")
                         .arg(directory);
        return false;
    }

    QDomDocument doc;
    doc.setContent(obj->toMemento(false).toByteArray());

    if (!gt::xml::writeDomDocumentToFile(path, doc, true))
    {
        gtError() << QObject::tr("GtExportToMementoCalculator: Could not save Object '%1' to '%2'").arg(obj->objectName(), path);
        return false;
    }

    gtInfo() << QObject::tr("Object '%1' successfully exported to '%2'")
                    .arg(obj->objectName(), path);

    return true;
}

GtCalculatorData
GtExportToMementoCalculator::calculatorData()
{
    GtCalculatorData exportMemento = GT_CALC_DATA(GtExportToMementoCalculator);
    exportMemento->id = QStringLiteral("Export Object to Memento");
    exportMemento->version = GtVersionNumber(1,0);
    exportMemento->status = GtCalculatorDataImpl::RELEASE;
    exportMemento->category = "Core";
    return exportMemento;
}

