/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include <gtest/gtest.h>

#include "gt_objectfiltermodel.h"
#include "gt_objectgroup.h"
#include "gt_objectmodel.h"
#include "gt_styledmodel.h"

#include "datamodel/test_propertycontainerobject.h"

class ExposedObjectFilterModel : public GtObjectFilterModel
{
public:
    using GtObjectFilterModel::flags;
};

TEST(TestGtObjectFilterModel, keepsAncestorPathVisibleAndDisablesNonMatchingAncestors)
{
    GtObjectGroup root;
    root.setObjectName("root");

    auto* parent = new GtObjectGroup;
    parent->setObjectName("alpha");
    ASSERT_TRUE(root.appendChild(parent));

    auto* child = new TestObject;
    child->setObjectName("project");
    ASSERT_TRUE(parent->appendChild(child));

    GtObjectModel srcModel;
    srcModel.setRootObject(&root);

    GtStyledModel styledModel;
    styledModel.setSourceModel(&srcModel);

    ExposedObjectFilterModel filterModel;
    EXPECT_EQ(filterModel.flags(QModelIndex()), Qt::ItemFlags{});
    filterModel.setSourceModel(&styledModel);
    filterModel.setFilterData({"TestObject"});
    filterModel.setFilterRegExp("pr");

    QModelIndex srcRootIndex = srcModel.indexFromObject(&root);
    QModelIndex styledRootIndex = styledModel.mapFromSource(srcRootIndex);
    QModelIndex filterRootIndex = filterModel.mapFromSource(styledRootIndex);

    ASSERT_TRUE(filterRootIndex.isValid());
    ASSERT_EQ(filterModel.rowCount(filterRootIndex), 1);

    QModelIndex parentIndex = filterModel.index(0, 0, filterRootIndex);
    ASSERT_TRUE(parentIndex.isValid());
    EXPECT_EQ(filterModel.data(parentIndex).toString(), QString("alpha"));
    EXPECT_EQ(filterModel.rowCount(parentIndex), 1);
    EXPECT_FALSE(filterModel.flags(parentIndex) & Qt::ItemIsEnabled);

    QModelIndex childIndex = filterModel.index(0, 0, parentIndex);
    ASSERT_TRUE(childIndex.isValid());
    EXPECT_EQ(filterModel.data(childIndex).toString(), QString("project"));
    EXPECT_TRUE(filterModel.flags(childIndex) & Qt::ItemIsEnabled);
}
