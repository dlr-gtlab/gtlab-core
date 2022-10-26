#include "gtest/gtest.h"

#include "gt_propertyreference.h"
#include "test_gt_object.h"
#include "test_propertycontainerobject.h"

struct TestPropertyReference : public testing::Test
{};

TEST_F(TestPropertyReference, normalProperty)
{
    bool ok = false;
    auto ref = GtPropertyReference::fromString("myProperty", ok);
    EXPECT_TRUE(ok);

    EXPECT_EQ("myProperty", ref.propertyId().toStdString());
    EXPECT_TRUE(ref.containerId().isEmpty());
    EXPECT_TRUE(ref.entryId().isEmpty());

    EXPECT_EQ("myProperty", ref.toString().toStdString());
}

TEST_F(TestPropertyReference, containerProperty)
{
    bool ok = false;
    auto ref = GtPropertyReference::fromString("containerID[{entry-id}].myProperty", ok);
    EXPECT_TRUE(ok);

    EXPECT_EQ("myProperty", ref.propertyId().toStdString());
    EXPECT_EQ("containerID", ref.containerId().toStdString());
    EXPECT_EQ("{entry-id}", ref.entryId().toStdString());

    EXPECT_EQ("containerID[{entry-id}].myProperty", ref.toString().toStdString());
}

TEST_F(TestPropertyReference, invalidRefs)
{
    bool ok = false;
    auto ref = GtPropertyReference::fromString("containerID.myProperty", ok);
    EXPECT_FALSE(ok);

    ref = GtPropertyReference::fromString("[entryID].myProperty", ok);
    EXPECT_FALSE(ok);

    ref = GtPropertyReference::fromString("containerID.entryID.myProperty", ok);
    EXPECT_FALSE(ok);
}

TEST_F(TestPropertyReference, resolveRegularProperty)
{
    bool ok=false;
    auto ref = GtPropertyReference::fromString("doubleProp", ok);
    ASSERT_TRUE(ok);

    TestSpecialGtObject obj;
    auto prop = ref.resolve(obj);
    ASSERT_TRUE(prop != nullptr);
    EXPECT_EQ("Test Double", prop->objectName().toStdString());
}

TEST_F(TestPropertyReference, resolveRegularPropertyFail)
{
    bool ok=false;
    auto ref = GtPropertyReference::fromString("_invalid_prop", ok);
    ASSERT_TRUE(ok);

    TestSpecialGtObject obj;
    auto prop = ref.resolve(obj);
    ASSERT_TRUE(prop == nullptr);
}

TEST_F(TestPropertyReference, resolveContainerProp)
{
    TestObject obj;
    obj.addEnvironmentVar("PATH", "/usr/bin");

    QString entryID = obj.environmentVars.at(0).ident();

    bool ok=false;
    auto ref = GtPropertyReference::fromString(
        QString("environmentVars[%1].value").arg(entryID), ok);

    ASSERT_TRUE(ok);

    auto prop = ref.resolve(obj);
    ASSERT_TRUE(prop != nullptr);
    EXPECT_EQ("/usr/bin", prop->valueToVariant().toString().toStdString());

    ref = GtPropertyReference::fromString(
        QString("environmentVars[%1].name").arg(entryID), ok);

    ASSERT_TRUE(ok);

    prop = ref.resolve(obj);
    ASSERT_TRUE(prop != nullptr);
    EXPECT_EQ("PATH", prop->valueToVariant().toString().toStdString());
}

TEST_F(TestPropertyReference, resolveContainerPropFail)
{
    TestObject obj;
    obj.addEnvironmentVar("PATH", "/usr/bin");

    QString entryID = obj.environmentVars.at(0).ident();

    bool ok=false;
    auto ref = GtPropertyReference::fromString(
        QString("environmentVars[%1]._does_not_exist_").arg(entryID), ok);
    ASSERT_TRUE(ok);

    auto prop = ref.resolve(obj);
    ASSERT_TRUE(prop == nullptr);

    ref = GtPropertyReference::fromString(
        QString("environmentVars[_invalid_entry_].value"), ok);
    ASSERT_TRUE(ok);

    prop = ref.resolve(obj);
    ASSERT_TRUE(prop == nullptr);

    ref = GtPropertyReference::fromString(
        QString("_invalid_container_[%1].value").arg(entryID), ok);

    ASSERT_TRUE(ok);

    prop = ref.resolve(obj);
    ASSERT_TRUE(prop == nullptr);
}
