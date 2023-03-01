/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_coredatamodel
 * copyright 2009-2016 by DLR
 *
 *  Created on: 02.09.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include "gt_coredatamodel.h"
#include "gt_object.h"

class TestCoreDataModel : public GtCoreDatamodel
{
public:
    TestCoreDataModel() : GtCoreDatamodel()
    {

    }

};

/// This is a test fixture that does a init for each test
class TestGtCoreDataModel : public ::testing::Test
{
protected:
    void SetUp() override
    {
        m_model = new TestCoreDataModel;
    }

    void TearDown() override
    {
        delete m_model;
    }

    GtCoreDatamodel* m_model;

};

TEST_F(TestGtCoreDataModel, instance)
{
    ASSERT_TRUE(m_model != nullptr);

    GtCoreDatamodel* model = gtDataModel;

    ASSERT_TRUE(m_model == model);

    GtCoreDatamodel* model2 = GtCoreDatamodel::instance();

    ASSERT_TRUE(m_model == model2);
}

TEST_F(TestGtCoreDataModel, getUniqueName)
{
    using s = std::string;
    std::vector<s> l = {"aa", "bb", "bb[2]", "cc", "cc[2]", "cc[3]"};

    auto f = [](const s& str) { return QString(str.c_str()); };

    EXPECT_EQ(s("unique"), getUniqueName("unique", l, f).toStdString());
    EXPECT_EQ(s("aa[2]"), getUniqueName("aa", l, f).toStdString());
    EXPECT_EQ(s("bb[3]"), getUniqueName("bb", l, f).toStdString());
    EXPECT_EQ(s("cc[4]"), getUniqueName("cc", l, f).toStdString());

    std::vector<QString> l2 = {"aa", "bb", "bb[2]", "cc", "cc[2]", "cc[3]"};

    EXPECT_EQ(s("unique"), getUniqueName("unique", l2).toStdString());
    EXPECT_EQ(s("aa[2]"), getUniqueName("aa", l2).toStdString());
    EXPECT_EQ(s("bb[3]"), getUniqueName("bb", l2).toStdString());
    EXPECT_EQ(s("cc[4]"), getUniqueName("cc", l2).toStdString());
}

TEST_F(TestGtCoreDataModel, makeUnqiueName)
{
    GtObject parent;
    GtObject* child1 = new GtObject;
    child1->setObjectName("Test");
    parent.appendChild(child1);
    GtObject* child2 = new GtObject;
    child2->setObjectName("Test[3]");
    parent.appendChild(child2);

    auto newName = gt::makeUniqueName("Test", parent);
    EXPECT_EQ(newName.toStdString(), "Test[2]");

    GtObject* child3 = new GtObject;
    child3->setObjectName(newName);
    parent.appendChild(child3);

    EXPECT_EQ(gt::makeUniqueName("Test", parent).toStdString(), "Test[4]");
}
