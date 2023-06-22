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

#include <QCoreApplication>
#include <QDir>
#include <QUuid>
#include <QDebug>

#include "gt_testhelper.h"
#include "gt_logging.h"

GtTestHelper::GtTestHelper()
{    
    QDir tempDir(tempPath());

    if (tempDir.exists())
    {
        tempDir.removeRecursively();
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
    QString uuid = QUuid::createUuid().toString();

    QString path = tempPath() + QDir::separator() + uuid;

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

QString
GtTestHelper::tempPath()
{
    return QCoreApplication::applicationDirPath() + QDir::separator() +
            QStringLiteral("unittest-temp");
}
