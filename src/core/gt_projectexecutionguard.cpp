/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_projectexecutionguard.h"

#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QSet>
#include <QtGlobal>

#include "gt_project.h"

namespace
{
QMutex& guardMutex()
{
    static QMutex mutex;
    return mutex;
}

QSet<QString>& guardedProjects()
{
    static QSet<QString> projects;
    return projects;
}
}

GtProjectExecutionGuard::~GtProjectExecutionGuard()
{
    release();
}

GtProjectExecutionGuard::Result
GtProjectExecutionGuard::tryAcquire(GtProject* project)
{
    release();

    const QString key = projectKey(project);
    if (key.isEmpty())
    {
        return Result::InvalidProject;
    }

    QMutexLocker locker(&guardMutex());
    if (guardedProjects().contains(key))
    {
        return Result::Busy;
    }

    guardedProjects().insert(key);
    m_key = key;
    return Result::Acquired;
}

void
GtProjectExecutionGuard::release()
{
    if (m_key.isEmpty())
    {
        return;
    }

    QMutexLocker locker(&guardMutex());
    guardedProjects().remove(m_key);
    m_key.clear();
}

bool
GtProjectExecutionGuard::isLocked() const
{
    return !m_key.isEmpty();
}

QString
GtProjectExecutionGuard::projectKey() const
{
    return m_key;
}

QString
GtProjectExecutionGuard::projectKey(GtProject const* project)
{
    if (!project)
    {
        return {};
    }

    if (!project->path().isEmpty())
    {
        QFileInfo info(project->path());
        QString path = info.canonicalFilePath();
        if (path.isEmpty())
        {
            path = info.absoluteFilePath();
        }

        return QDir::cleanPath(path);
    }

    if (!project->uuid().isEmpty())
    {
        return QStringLiteral("uuid:") + project->uuid();
    }

    return QStringLiteral("object:%1")
        .arg(reinterpret_cast<quintptr>(project), 0, 16);
}

bool
GtProjectExecutionGuard::isLocked(GtProject const* project)
{
    const QString projectPath = projectKey(project);
    if (projectPath.isEmpty())
    {
        return false;
    }

    QMutexLocker locker(&guardMutex());
    return guardedProjects().contains(projectPath);
}
