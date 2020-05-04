/* GTlab - Gas Turbine laboratory
 * Source File: gt_testhelper.cpp
 * copyright 2009-2016 by DLR
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

GtTestHelper::GtTestHelper()
{

}

GtTestHelper*
GtTestHelper::instance()
{
    static GtTestHelper* retval = 0;
    if (retval == 0)
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

GtTestHelper::~GtTestHelper()
{
    QDir tempDir(tempPath());

    if (tempDir.exists())
    {
        tempDir.removeRecursively();
    }
}

QString
GtTestHelper::tempPath()
{
    return QCoreApplication::applicationDirPath() + QDir::separator() +
            QStringLiteral("unittest-temp");
}
