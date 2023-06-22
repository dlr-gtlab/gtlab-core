/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_updatechecker.cpp
 *
 *  Created on: 20.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QCoreApplication>
#include <QProcess>
#include <QDomDocument>

#include "gt_globals.h"
#include "gt_logging.h"

#include "gt_updatechecker.h"

GtUpdateChecker::GtUpdateChecker(QObject* parent) : QObject(parent),
    m_extendedInfo(false),
    m_errorCode(-100)
{

}

void
GtUpdateChecker::enableExtendedInfo(bool val)
{
    m_extendedInfo = val;
}

void
GtUpdateChecker::checkForUpdate()
{
    m_pkgList.clear();
    m_errorCode = updateHelper();

    if (m_errorCode != 0)
    {
        emit error(m_errorCode, m_errorMessage);
    }
    else
    {
        emit updateAvailable();
    }
}

int
GtUpdateChecker::updateHelper()
{
    gtDebug().medium() << "Update check...";
    QDir path(qApp->applicationDirPath());

    if (!path.cdUp())
    {
        m_errorMessage = tr("Maintanance tool not found!");
        return 1;
    }

    QString maintainance(QStringLiteral(GT_MAINTENANCETOOL));

#ifdef Q_OS_WIN32
    maintainance = QStringLiteral(GT_MAINTENANCETOOL) +
                   QStringLiteral(".exe");
#endif

    if (!QFile(path.absoluteFilePath(maintainance)).exists())
    {
        m_errorMessage = tr("Maintanance tool not found!");
        return 1;
    }

    QProcess ps;
    QString progName = path.absolutePath() + QDir::separator() +
                       maintainance;
    QStringList arguments;
    arguments << QStringLiteral("ch");
    ps.start(progName, arguments);

    gtDebug() << "running maintenance tool...";

    if (!ps.waitForFinished(30000))
    {
        m_errorMessage = tr("Timed out!");
        return 2;
    }

    if (ps.exitCode() != 0)
    {
        m_errorMessage = tr("Could not establish connection to update server!");
        return 3;
    }

    gtDebug() << "connection established! reading update info...";

    QString outStr = QLatin1String(ps.readAllStandardOutput());

    if (!outStr.contains(QStringLiteral("<updates>")))
    {
        m_errorMessage = tr("No updates available!");
        return 6;
    }

    if (m_extendedInfo)
    {
        int firstIdx = outStr.indexOf(QStringLiteral("<updates>"));
        int lastIdx = outStr.lastIndexOf(QStringLiteral("</updates>"));

        QString outStrTrimmed = outStr.mid(firstIdx, lastIdx);

        QString errorStr;
        int errorLine;
        int errorColumn;

        QDomDocument document;
        if (!document.setContent(outStrTrimmed, true, &errorStr, &errorLine, &errorColumn))
        {
            gtError() << "update data error!";
            gtError() << "  |-> " << errorLine << ";" << errorColumn << " - " <<
                         errorStr;

            m_errorMessage = tr("Update data error!");
            return 4;
        }

        QDomElement elm_root = document.documentElement();

        QDomElement pkg_element_it =
                elm_root.firstChildElement(QStringLiteral("update"));

        while (!pkg_element_it.isNull())
        {
            PackageInfo pkg_info;
            pkg_info.m_id = pkg_element_it.attribute(QStringLiteral("id"));
            pkg_info.m_name = pkg_element_it.attribute(QStringLiteral("name"));
            pkg_info.m_newVers =
                    pkg_element_it.attribute(QStringLiteral("version"));

            if (!readExtendedInfo(progName, pkg_info))
            {
                m_errorMessage = tr("Update data error!");
                return 4;
            }

            m_pkgList << pkg_info;

            pkg_element_it =
                    pkg_element_it.nextSiblingElement(QStringLiteral("update"));
        }

        gtDebug() << "update available for:";

        foreach (const PackageInfo& pkg_info, m_pkgList)
        {
            gtDebug() << "- " << pkg_info.m_name << " (" <<
                         pkg_info.m_currentVers << " -> " <<

                         pkg_info.m_newVers << ")";
        }

    }

    return 0;
}

QList<GtUpdateChecker::PackageInfo>
GtUpdateChecker::pkgList() const
{
    return m_pkgList;
}

bool
GtUpdateChecker::readExtendedInfo(const QString& mTool,
                                  GtUpdateChecker::PackageInfo& pkgInfo)
{
    QProcess ps;

    QStringList arguments;
    arguments << QStringLiteral("li");
    arguments << pkgInfo.m_id;

    ps.start(mTool, arguments);

    gtDebug() << "running maintenance tool...";

    if (!ps.waitForFinished(30000))
    {
        return false;
    }

    QString outStr = QLatin1String(ps.readAllStandardOutput());

    int firstIdx = outStr.indexOf(QStringLiteral("<localpackages>"));
    int lastIdx = outStr.lastIndexOf(QStringLiteral("</localpackages>"));

    QString outStrTrimmed = outStr.mid(firstIdx, lastIdx);

    QString errorStr;
    int errorLine;
    int errorColumn;

    QDomDocument document;
    if (!document.setContent(outStrTrimmed, true, &errorStr, &errorLine, &errorColumn))
    {
        gtError() << "update data error!";
        gtError() << "  |-> " << errorLine << ";" << errorColumn << " - " <<
                     errorStr;

        return false;
    }

    QDomElement elm_root = document.documentElement();

    QDomElement pkg_element_it =
            elm_root.firstChildElement(QStringLiteral("package"));

    while (!pkg_element_it.isNull())
    {
        pkgInfo.m_currentVers =
                pkg_element_it.attribute(QStringLiteral("version"));

        pkg_element_it =
                pkg_element_it.nextSiblingElement(QStringLiteral("package"));
    }

    return true;
}
