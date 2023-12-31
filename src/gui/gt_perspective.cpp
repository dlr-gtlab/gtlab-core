/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>

#include "gt_logging.h"
#include "gt_perspective.h"
#include "gt_application.h"

GtPerspective::GtPerspective(const QString& id)
{
    setObjectName(id);
}

bool
GtPerspective::createDefault()
{
    QString path = gtApp->roamingPath() + QDir::separator() +
                   QStringLiteral("perspective");

    if (!QDir().mkpath(path))
    {
        gtFatal() << tr("Could not create application directories!");
        return false;
    }

    return createEmptyPerspective(QStringLiteral("default"));
}

bool
GtPerspective::createEmptyPerspective(const QString& id)
{
    QString path = gtApp->roamingPath() + QDir::separator() +
                   QStringLiteral("perspective");

    QDir dir(path);

    if (!dir.exists())
    {
        gtFatal() << tr("Roaming path not found!");
        return false;
    }

    if (!emptyFile(dir, id + QStringLiteral(".geom")))
    {
        return false;
    }

    if (!emptyFile(dir, id + QStringLiteral(".state")))
    {
        return false;
    }

    return true;
}

bool
GtPerspective::duplicatePerspective(const QString& source,
                                    const QString& target)
{
    QString path = gtApp->roamingPath() + QDir::separator() +
                   QStringLiteral("perspective");

    QDir dir(path);

    if (!dir.exists())
    {
        gtFatal() << tr("Roaming path not found!");
        return false;
    }

    QFile file1(dir.absoluteFilePath(source + QStringLiteral(".geom")));

    if (!file1.exists())
    {
        gtError() << tr("Perspective geometry file not found!");
        return false;
    }

    QString targetFilename1 = target + QStringLiteral(".geom");

    if (!file1.copy(dir.absoluteFilePath(targetFilename1)))
    {
        gtError() << tr("Could not copy perspective geometry file not found!");
        return false;
    }

    QFile file2(dir.absoluteFilePath(source + QStringLiteral(".state")));

    if (!file2.exists())
    {
        gtError() << tr("Perspective state file not found!");
        return false;
    }

    QString targetFilename2 = target + QStringLiteral(".state");

    if (!file2.copy(dir.absoluteFilePath(targetFilename2)))
    {
        gtError() << tr("Could not copy perspective state file not found!");
        return false;
    }

    return true;
}

void
GtPerspective::saveGeometry(const QByteArray& data)
{
    QString path = gtApp->roamingPath() + QDir::separator() +
                   QStringLiteral("perspective");

    QDir dir(path);

    if (!dir.exists())
    {
        gtError() << tr("roaming path not found!");
        return;
    }

    QString persFilename = objectName() + QStringLiteral(".geom");

    QFile file(dir.absoluteFilePath(persFilename));

    if (!file.open(QIODevice::WriteOnly))
    {
        gtError() << tr("could not create perspective file!");
        return;
    }

    file.write(data);

    file.close();
}

void
GtPerspective::saveState(const QByteArray& data)
{
    QString path = gtApp->roamingPath() + QDir::separator() +
                   QStringLiteral("perspective");

    QDir dir(path);

    if (!dir.exists())
    {
        gtError() << tr("roaming path not found!");
        return;
    }

    QString persFilename = objectName() + QStringLiteral(".state");

    QFile file(dir.absoluteFilePath(persFilename));

    if (!file.open(QIODevice::WriteOnly))
    {
        gtError() << tr("could not create perspective file!");
        return;
    }

    file.write(data);

    file.close();
}

QByteArray
GtPerspective::loadGeometry()
{
    QString path = gtApp->roamingPath() + QDir::separator() +
                   QStringLiteral("perspective");

    QDir dir(path);

    if (!dir.exists())
    {
        gtFatal() << tr("roaming path not found!");
        return QByteArray();
    }

    QString persFilename = objectName() + QStringLiteral(".geom");

    QFile file(dir.absoluteFilePath(persFilename));

    if (!file.open(QIODevice::ReadOnly))
    {
        gtError() << tr("Could not create perspective file!");
        return QByteArray();
    }

    QByteArray data = file.readAll();

    file.close();

    return data;
}

QByteArray
GtPerspective::loadState()
{
    QString path = gtApp->roamingPath() + QDir::separator() +
                   QStringLiteral("perspective");

    QDir dir(path);

    if (!dir.exists())
    {
        gtFatal() << tr("Roaming path not found!");
        return QByteArray();
    }

    QString persFilename = objectName() + QStringLiteral(".state");

    QFile file(dir.absoluteFilePath(persFilename));

    if (!file.open(QIODevice::ReadOnly))
    {
        gtError() << tr("Could not create perspective file!");
        return QByteArray();
    }

    QByteArray data = file.readAll();

    file.close();

    return data;
}

bool
GtPerspective::emptyFile(const QDir& dir, const QString& filename)
{
    QFile file(dir.absoluteFilePath(filename));

    if (file.exists())
    {
        // file found -> nothing to do here
        return true;
    }

    if (!file.open(QIODevice::WriteOnly))
    {
        gtError() << tr("Could not create perspective file!");
        return false;
    }

    file.write(QByteArray());

    file.close();

    return true;
}

