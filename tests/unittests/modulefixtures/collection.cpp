/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include <QIcon>
#include <QMap>

#include "gt_collectioninterface.h"
#include "gt_collectionsettings.h"
#include "gt_localcollectionmodel.h"
#include "gt_moduleinterface.h"

namespace
{

class CustomCollectionModel : public GtLocalCollectionModel
{
public:
    QVariant data(const QModelIndex& index, int role) const override
    {
        if (index.isValid() && index.column() == 0 && role == Qt::DisplayRole)
        {
            return QStringLiteral("Custom collection item");
        }

        return GtLocalCollectionModel::data(index, role);
    }
};

} // namespace

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

    Q_INVOKABLE QObject* createCollectionModel() const
    {
        auto* model = new CustomCollectionModel;
        model->setItems({GtCollectionItem{}});
        return model;
    }
};

#include "collection.moc"
