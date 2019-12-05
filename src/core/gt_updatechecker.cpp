/* GTlab - Gas Turbine laboratory
 * Source File: gt_updatechecker.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 20.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QCoreApplication>
#include <QProcess>

#include "gt_logging.h"

#include "gt_updatechecker.h"

GtUpdateChecker::GtUpdateChecker(QObject* parent) : QObject(parent)
{

}

void
GtUpdateChecker::checkForUpdate()
{
    gtDebug() << "update check...";
    QDir path(qApp->applicationDirPath());

    if (!path.cdUp())
    {
        emit error(1, tr("Maintanance tool not found!"));
        return;
    }

    QString maintainance(QStringLiteral("MaintenanceTool"));

#ifdef Q_OS_WIN32
    maintainance = QStringLiteral("MaintenanceTool.exe");
#endif

    if (!QFile(path.absoluteFilePath(maintainance)).exists())
    {
        emit error(1, tr("Maintanance tool not found!"));
        return;
    }

    QProcess ps;
    QString progName = path.absolutePath() + QDir::separator() +
                       maintainance;
    QStringList arguments;
    arguments << QStringLiteral("--checkupdates");
    ps.start(progName, arguments);

    gtDebug() << "running maintenance tool...";

    if (!ps.waitForFinished(30000))
    {
        gtDebug() << "timed out!";
        emit error(2, tr("Timed out!"));
        return;
    }

    QFile file(path.absoluteFilePath(QStringLiteral("InstallationLog.txt")));

    if (!file.exists())
    {
        emit error(3, tr("Update file not found!"));
        return;
    }

    if (!file.open(QFile::ReadOnly))
    {
        emit error(4, tr("Could not open update file!"));
        return;
    }

    QString data = QString::fromLatin1(file.readAll());
    QStringList rowdata = data.split(QRegExp(QStringLiteral("[\r\n]")),
                                     QString::SkipEmptyParts);

    gtDebug() << "last line = " << rowdata.last();

    if (rowdata.last().contains(QStringLiteral("Connection refused")))
    {
        emit error(5, tr("Connection refused!"));
        return;
    }
    else if (rowdata.last().contains(QStringLiteral("no updates available")))
    {
        emit error(6, tr("No updates available!"));
        return;
    }
    else if (rowdata.last().contains(QStringLiteral("Failure")))
    {
        emit error(7, rowdata.last());
        return;
    }
    else if (rowdata.last().contains(QStringLiteral("Error")))
    {
        emit error(8, rowdata.last());
        return;
    }

    file.close();

    emit updateAvailable();
}
