/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_collectionitem.h"
#include "gt_collectionloader.h"
#include "gt_icons.h"
#include "gt_localcollectionmodel.h"

#include <QAbstractItemModel>
#include <QIcon>
#include <QPluginLoader>

#include <memory>

TEST(CollectionModel, ModuleCanOverrideDisplayRole)
{
    QPluginLoader loader(QString::fromUtf8(COLLECTION_FIXTURE_PATH));
    auto* plugin = loader.instance();
    ASSERT_NE(nullptr, plugin) << loader.errorString().toStdString();

    QObject* modelObject = nullptr;
    ASSERT_TRUE(QMetaObject::invokeMethod(plugin, "createCollectionModel",
                                          Q_RETURN_ARG(QObject*, modelObject)));
    std::unique_ptr<QObject> modelOwner{modelObject};
    auto* model = qobject_cast<QAbstractItemModel*>(modelOwner.get());
    ASSERT_NE(nullptr, model);

    ASSERT_EQ(1, model->rowCount());
    EXPECT_EQ(QStringLiteral("Custom collection item"),
              model->index(0, 0).data().toString());
}

TEST(CollectionModel, CustomIconsArePreservedPerItem)
{
    GtCollectionItem first;
    GtCollectionItem second;
    const QIcon firstIcon = gt::gui::icon::error();
    const QIcon secondIcon = gt::gui::icon::info2();
    first.setIcon(firstIcon);
    second.setIcon(secondIcon);

    GtLocalCollectionModel model;
    model.setItems({first, second});

    ASSERT_EQ(2, model.rowCount(QModelIndex()));
    EXPECT_EQ(firstIcon.cacheKey(),
              model.data(model.index(0, 0), Qt::DecorationRole)
                  .value<QIcon>()
                  .cacheKey());
    EXPECT_EQ(secondIcon.cacheKey(),
              model.data(model.index(1, 0), Qt::DecorationRole)
                  .value<QIcon>()
                  .cacheKey());
    EXPECT_EQ(firstIcon.cacheKey(),
              model.itemFromIndex(model.index(0, 0)).icon().cacheKey());
}

TEST(CollectionModel, CustomIconDoesNotReplaceInformationColumn)
{
    GtCollectionItem item;
    item.setIcon(gt::gui::icon::error());

    GtLocalCollectionModel model;
    model.setItems({item});

    EXPECT_EQ(gt::gui::icon::info2().icon().cacheKey(),
              model.data(model.index(0, 1), Qt::DecorationRole)
                  .value<QIcon>()
                  .cacheKey());
    EXPECT_FALSE(model.data(model.index(0, 2), Qt::DecorationRole).isValid());
    EXPECT_FALSE(model.data(QModelIndex(), Qt::DecorationRole).isValid());
}

TEST(CollectionModel, ReplacingItemsReplacesTheirIcons)
{
    GtCollectionItem original;
    original.setIcon(gt::gui::icon::error());
    GtCollectionItem replacement;
    replacement.setIcon(gt::gui::icon::info2());

    GtLocalCollectionModel model;
    model.setItems({original});
    model.setItems({replacement});

    ASSERT_EQ(1, model.rowCount(QModelIndex()));
    EXPECT_EQ(replacement.icon().cacheKey(),
              model.data(model.index(0, 0), Qt::DecorationRole)
                  .value<QIcon>()
                  .cacheKey());

    model.setItems({});
    EXPECT_EQ(0, model.rowCount(QModelIndex()));
    EXPECT_FALSE(model.data(QModelIndex(), Qt::DecorationRole).isValid());
}

TEST(CollectionLoader, UnknownCollectionHasNoPathOrItems)
{
    GtCollectionLoader loader(
        QStringLiteral("unregistered-unit-test-collection"));

    EXPECT_TRUE(loader.collectionPath().isEmpty());
    EXPECT_TRUE(loader.loadLocalCollection().isEmpty());
}
