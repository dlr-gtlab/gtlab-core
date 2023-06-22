/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 7.10.2022
 *  Author: Martin Siggel (AT-TWK)
 *  E-Mail: martin.siggel@dlr.de
 */

#include "test_issue358.h"

#include <gtest/gtest.h>

#include <gt_objectmemento.h>
#include <gt_objectfactory.h>

/**
 * About this test:
 *
 * When objects are beeing created by the memento restore function,
 * they might already rely on the existence of a parent object
 *
 * A previous implementation was adding the parent only after object
 * creation, leading to the bug 358.
 *
 * This test checks, that the parent is already added to the object
 * before reading in all other properties by the memento::toObject
 * function.
 */

class Test358factory : public GtAbstractObjectFactory {};

TestObject358::TestObject358() :
    dp("testval", "testval")
{
    registerProperty(dp);

    dp.setVal(1.0);

    connect(&dp, &GtDoubleProperty::changed, this, [this]() {
        valueHasChanged = true;
        parentExist = parentObject() != nullptr;
    });
}

/**
 * Check that lambda is executed
 */
TEST(Issue358, noParentBaseline)
{
    TestObject358 o;
    o.setValue(2.0);

    EXPECT_TRUE(o.valueHasChanged);
    EXPECT_FALSE(o.parentExistedOnValueChanged());
}

/**
 * Check that lambda is executed
 * and parent was added before
 */
TEST(Issue358, withParentBaseline)
{
    GtObject parent;
    auto o = new TestObject358;
    parent.appendChild(o);
    o->setValue(2.0);

    EXPECT_TRUE(o->valueHasChanged);
    EXPECT_TRUE(o->parentExistedOnValueChanged());
}

/**
 * Verify, that issue 358 is fixed
 */
TEST(Issue358, checkParentExistOnMementoRestore)
{
    Test358factory factory;
    factory.registerClass(TestObject358::staticMetaObject);

    auto mementoStr = R"(
<object uuid="{parentUID}" class="GtObject">
 <objectlist>
  <object uuid="{childUID}" class="TestObject358">
   <property type="double" name="testval">2</property>
  </object>
 </objectlist>
</object>
    )";

    const auto memento = GtObjectMemento(QByteArray(mementoStr));
    ASSERT_FALSE(memento.isNull());

    const auto parent = memento.toObject(factory);
    ASSERT_TRUE(parent != nullptr);

    const auto o = parent->findDirectChild<TestObject358*>();
    ASSERT_TRUE(o != nullptr);

    EXPECT_TRUE(o->valueHasChanged);

    // If true, the issue is fixed
    EXPECT_TRUE(o->parentExistedOnValueChanged());
}
