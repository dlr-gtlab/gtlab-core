/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_projectio.h"

#include "gt_xmlutilities.h"
#include "gt_logging.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDomDocument>

QDomDocument
GtProjectIO::readProjectData(const QDir& projectPath)
{
    QString filename = projectPath.filePath(QStringLiteral("project.gtlab"));

    QFile file(filename);

    if (!file.exists())
    {
        qWarning() << "WARNING: file does not exists!";
        qWarning() << " |-> " << filename;
        return QDomDocument();
    }

    QDomDocument document;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!gt::xml::readDomDocumentFromFile(file, document, true, &errorStr,
                                          &errorLine, &errorColumn))
    {
        gtDebug() << QCoreApplication::tr("XML ERROR!") << " " << QCoreApplication::tr("line") << ": "
                  << errorLine << " " << QCoreApplication::tr("column") << ": "
                  << errorColumn << " -> " << errorStr;
        return QDomDocument();
    }

    QDomElement root = document.documentElement();

    if (root.isNull() || (root.tagName() != QLatin1String("GTLAB")))
    {
        gtDebug() << "ERROR: Invalid GTlab project file!";
        return QDomDocument();
    }

    return document;
}

bool
GtProjectIO::saveProjectFiles(const QString& filePath,
                              const QDomDocument& doc,
                              bool saveWithLinkedFiles)
{
    const QFileInfo fi(filePath);
    const QDir baseDir = fi.dir().absolutePath();

    QString error;
    if (!gt::xml::saveProjectXmlWithLinkedObjects(QLatin1String("project"),
                                                  doc,
                                                  baseDir,
                                                  filePath,
                                                  saveWithLinkedFiles ?
                                                      gt::xml::LinkFileSaveType::WithLinkedFiles :
                                                      gt::xml::LinkFileSaveType::OneFile,
                                                  &error))
    {
        gtError() << error;
        return false;
    }

    return true;
}

bool
GtProjectIO::fileExists(const QString& path)
{
    return QFile::exists(path);
}
