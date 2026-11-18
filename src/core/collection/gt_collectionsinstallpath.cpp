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


namespace gt
{
namespace detail
{

const std::optional<const QString> collectionsInstallPath(bool mkpath)
{
    const auto* settings = gtApp->settings();
    QString userCollectionsDir = settings->userCollectionsDir();

    QString collectionsPathActual = "";

    if (userCollectionsDir.isEmpty())
    {

        QString error;
        const auto dataDirectory = GtApplicationPaths::directory(
            GtApplicationPaths::Location::Data, &error);
        if (!dataDirectory)
        {
            gtError() << QObject::tr("Could not access collection data directory:") << error;
            return {};
        }

        collectionsPathActual = dataDirectory->absoluteFilePath("collections");
    }
    else
    {
        collectionsPathActual = userCollectionsDir;
    }

    if (QFileInfo{collectionsPathActual}.isFile())
    {
        gtError() << QObject::tr("Retrieved collections path is a file");
        return {};
    }

    QDir collectionsDir{collectionsPathActual};

    if (mkpath && !collectionsDir.mkpath("."))
    {
        gtError() << QObject::tr("Could not create collection root path!");
        return {};
    }

    return collectionsDir.absolutePath();
}


}
}
