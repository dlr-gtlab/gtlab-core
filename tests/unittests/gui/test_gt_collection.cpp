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

#include <QIcon>

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

    ASSERT_EQ(2, model.rowCount());
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

    EXPECT_FALSE(model.data(model.index(0, 1), Qt::DecorationRole)
                     .value<QIcon>()
                     .isNull());
    EXPECT_NE(item.icon().cacheKey(),
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

    ASSERT_EQ(1, model.rowCount());
    EXPECT_EQ(replacement.icon().cacheKey(),
              model.data(model.index(0, 0), Qt::DecorationRole)
                  .value<QIcon>()
                  .cacheKey());

    model.setItems({});
    EXPECT_EQ(0, model.rowCount());
    EXPECT_FALSE(model.data(QModelIndex(), Qt::DecorationRole).isValid());
}

TEST(CollectionLoader, UnknownCollectionHasNoPathOrItems)
{
    GtCollectionLoader loader(
        QStringLiteral("unregistered-unit-test-collection"));

    EXPECT_TRUE(loader.collectionPath().isEmpty());
    EXPECT_TRUE(loader.loadLocalCollection().isEmpty());
}
