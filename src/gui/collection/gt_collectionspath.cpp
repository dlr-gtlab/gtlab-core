#include "gt_collectionspath.h"

#include "gt_applicationpaths.h"
#include "gt_coreapplication.h"
#include "gt_settings.h"
#include "gt_logging.h"

const QString gt::detail::collectionsInstallPath(bool mkpath)
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
            gtError() << QObject::tr("Could not access collection data directory:")
            << error;
            return QString();
        }

        collectionsPathActual = dataDirectory->absoluteFilePath("collections");
    }
    else
    {
        collectionsPathActual = userCollectionsDir;
    }

    QDir collectionsDir{collectionsPathActual};

    if (mkpath && !collectionsDir.mkpath("."))
    {
        gtError() << QObject::tr("Could not create collection root path!");
        return {};
    }

    return collectionsDir.absolutePath();
}
