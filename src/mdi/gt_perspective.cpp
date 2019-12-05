/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QDebug>

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
        qWarning() << tr("WARNING") << ": "
                   << tr("could not create application directories!");
        return false;
    }

    return createEmptyPerspective(QStringLiteral("default"));
}

bool
GtPerspective::createEmptyPerspective(const QString id)
{
    QString path = gtApp->roamingPath() + QDir::separator() +
                   QStringLiteral("perspective");

    QDir dir(path);

    if (!dir.exists())
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("roaming path not found!");
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
GtPerspective::duplicatePerspective(const QString source, const QString target)
{
    QString path = gtApp->roamingPath() + QDir::separator() +
                   QStringLiteral("perspective");

    QDir dir(path);

    if (!dir.exists())
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("roaming path not found!");
        return false;
    }

    QFile file1(dir.absoluteFilePath(source + QStringLiteral(".geom")));

    if (!file1.exists())
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("Perspective geometry file not found!");
        return false;
    }

    QString targetFilename1 = target + QStringLiteral(".geom");

    if (!file1.copy(dir.absoluteFilePath(targetFilename1)))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("Could not copy perspective geometry file not found!");
        return false;
    }

    QFile file2(dir.absoluteFilePath(source + QStringLiteral(".state")));

    if (!file2.exists())
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("Perspective state file not found!");
        return false;
    }

    QString targetFilename2 = target + QStringLiteral(".state");

    if (!file2.copy(dir.absoluteFilePath(targetFilename2)))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("Could not copy perspective state file not found!");
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
        qWarning() << tr("WARNING") << ": "
                   << tr("roaming path not found!");
        return;
    }

    QString persFilename = objectName() + QStringLiteral(".geom");

    QFile file(dir.absoluteFilePath(persFilename));

    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not create perspective file!");
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
        qWarning() << tr("WARNING") << ": "
                   << tr("roaming path not found!");
        return;
    }

    QString persFilename = objectName() + QStringLiteral(".state");

    QFile file(dir.absoluteFilePath(persFilename));

    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not create perspective file!");
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
        qWarning() << tr("WARNING") << ": "
                   << tr("roaming path not found!");
        return QByteArray();
    }

    QString persFilename = objectName() + QStringLiteral(".geom");

    QFile file(dir.absoluteFilePath(persFilename));

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not create perspective file!");
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
        qWarning() << tr("WARNING") << ": "
                   << tr("roaming path not found!");
        return QByteArray();
    }

    QString persFilename = objectName() + QStringLiteral(".state");

    QFile file(dir.absoluteFilePath(persFilename));

    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not create perspective file!");
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
//        qWarning() << tr("WARNING") << ": "
//                   << tr("default perspective file already exists!");
        return false;
    }

    if (!file.open(QIODevice::WriteOnly))
    {
        qWarning() << tr("WARNING") << ": "
                   << tr("could not create perspective file!");
        return false;
    }

    file.write(QByteArray());

    file.close();

    return true;
}

