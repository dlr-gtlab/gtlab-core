/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_collectionsinstallpath.h"

#include "gt_applicationpaths.h"
#include "gt_coreapplication.h"
#include "gt_settings.h"
#include "gt_logging.h"

#include <QDir>
#include <QFileInfo>


namespace gt
{
namespace detail
{


const QString
collectionsInstallDefaultPath()
{
    QString error;
    const auto dataDirectory = GtApplicationPaths::directory(
        GtApplicationPaths::Location::Data, &error);
    if (!dataDirectory)
    {
        gtError() << QObject::tr("Could not access collection data directory:")
                  << error;
        return {};
    }

    return dataDirectory->absoluteFilePath(QStringLiteral("Collections"));
}

const QString
collectionsInstallPath(bool mkpath, bool* ok)
{
    if (ok)
    {
        *ok = false;
    }

    const auto* settings = gtApp->settings();
    QString userCollectionsDir = settings->userCollectionsDir();

    QString collectionsPathActual;

    if (userCollectionsDir.isEmpty())
    {
        collectionsPathActual = collectionsInstallDefaultPath();
    }
    else
    {
        collectionsPathActual = userCollectionsDir;
    }

    if (collectionsPathActual.isEmpty())
    {
        gtError() << QObject::tr("Collections path is empty");
        return collectionsPathActual;
    }

    if (QFileInfo{collectionsPathActual}.isFile())
    {
        gtError() << QObject::tr("Retrieved collections path is a file");
        return collectionsPathActual;
    }

    QDir collectionsDir{collectionsPathActual};

    if (mkpath && !collectionsDir.mkpath("."))
    {
        gtError() << QObject::tr("Could not create collection root path!");
        return collectionsPathActual;
    }

    if (ok)
    {
        *ok = true;
    }
    return collectionsDir.absolutePath();
}

QString
collectionInstallPath(const QString& collectionId)
{
    bool ok;
    const auto installPath = collectionsInstallPath(true, &ok);
    if (!ok)
    {
        gtError() << QObject::tr("Could not resolve collections install path!");
        return {};
    }

    const QDir collectionsDir{installPath};
    if (!collectionsDir.mkpath(collectionId))
    {
        gtError() << QObject::tr("Could not create collection path!");
        return {};
    }

    return QDir::cleanPath(collectionsDir.filePath(collectionId));
}


} // namespace detail
} // namespace gt
