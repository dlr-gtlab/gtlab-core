/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_testhelper.cpp
 *
 *  Created on: 08.09.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QUuid>
#include <QDebug>

#include "gt_testhelper.h"
#include "gt_logging.h"

GtTestHelper::GtTestHelper() :
    m_tempDirectory(
        QDir(QDir::tempPath()).filePath(QStringLiteral("GTlabUnitTest-XXXXXX")))
{
    if (!m_tempDirectory.isValid())
    {
        qCritical() << "ERROR: Could not create test temporary directory!";
    }

     // setup gt logging
     gt::log::Logger& logger = gt::log::Logger::instance();
     auto dest = gt::log::makeDebugOutputDestination();
     logger.addDestination("console", std::move(dest));
}

GtTestHelper*
GtTestHelper::instance()
{
    static GtTestHelper* retval = nullptr;
    if (!retval)
    {
        retval = new GtTestHelper();
    }
    return retval;
}

QDir
GtTestHelper::newTempDir()
{
    if (!m_tempDirectory.isValid())
    {
        return QDir();
    }

    QString uuid = QUuid::createUuid().toString();

    QString path = QDir(m_tempDirectory.path()).filePath(uuid);

    QDir retval(path);

    if (!retval.mkpath(path))
    {
        qCritical() << "ERROR: Could not create temporary directory!";
        return QDir();
    }

    return retval;
}

QStringList
GtTestHelper::randomStringList(int length)
{
    QStringList retVal;
    retVal.reserve(length);

    for (int i = 0; i < length; ++i)
    {
        retVal.append(QUuid::createUuid().toString());
    }

    return retVal;
}

GtTestHelper::~GtTestHelper()
{
}
