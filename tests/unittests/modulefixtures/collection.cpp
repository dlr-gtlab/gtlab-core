/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include <QIcon>
#include <QMap>

#include "gt_collectioninterface.h"
#include "gt_collectionsettings.h"
#include "gt_moduleinterface.h"

class CollectionTestModule : public QObject,
                             public GtModuleInterface,
                             public GtCollectionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID GT_MODULE_ID)
    GT_IMPLEMENT_MODULE_
    Q_INTERFACES(GtCollectionInterface)

public:
    GtVersionNumber version() override
    {
        return {1, 0, 0};
    }
    QString description() const override
    {
        return "Collection path test fixture";
    }
    QIcon collectionIcon() const override
    {
        return {};
    }
    QString collectionId() const override
    {
        return "CollectionPathTest";
    }
    QMetaObject collectionSettings() const override
    {
        return GtCollectionSettings::staticMetaObject;
    }
};

#include "collection.moc"
