/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_structproperty
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.06.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include <QVariant>

#include "gt_dynamicpropertycontainer.h"
#include "gt_doubleproperty.h"

/// This is a test fixture that does a init for each test
class TestGtStructProperty : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_prop = new GtDynamicPropertyContainer("test struct");
    }

    virtual void TearDown()
    {
        delete m_prop;
    }

    GtDynamicPropertyContainer* m_prop;

};

TEST_F(TestGtStructProperty, initialization)
{
    ASSERT_EQ(m_prop->unitCategory(), GtUnit::Category::None);
    ASSERT_TRUE(m_prop->brief().isEmpty());
}

TEST_F(TestGtStructProperty, setFromVariant)
{
    bool success = false;
    ASSERT_FALSE(m_prop->setValueFromVariant(QVariant(), QString(), &success));
    ASSERT_FALSE(success);
}

TEST_F(TestGtStructProperty, getFromVariant)
{
    bool success = false;
    QVariant var = m_prop->valueToVariant(QString(), &success);
    ASSERT_TRUE(var.isNull());
    ASSERT_FALSE(success);
}

TEST_F(TestGtStructProperty, buildPoint3d)
{
    GtPropertyStructDefinition point3dClz("point_3d");

    auto makeDoubleProperty = [](double value) {
        return [=](const QString& id) {
            auto p = new GtDoubleProperty(id, id);
            p->setValueFromVariant(value, "");
            return p;
        };
    };

    point3dClz.defineMember("x", makeDoubleProperty(1.0));
    point3dClz.defineMember("y", makeDoubleProperty(2.0));

    // lets define z via the property factory
    point3dClz.defineMember("z", "GtDoubleProperty", 3.0);

    auto p1 = point3dClz.newInstance("p1");

    EXPECT_EQ("point_3d", p1->typeName().toStdString());
    EXPECT_EQ("p1", p1->ident().toStdString());


    EXPECT_TRUE(p1->findProperty("_this_does_not_exist_") == nullptr);

    auto props = p1->properties();
    EXPECT_EQ(3, props.size());
    EXPECT_EQ("x", props[0]->ident().toStdString());
    EXPECT_EQ("y", props[1]->ident().toStdString());
    EXPECT_EQ("z", props[2]->ident().toStdString());

    auto px = p1->findProperty("x");
    auto py = p1->findProperty("y");
    auto pz = p1->findProperty("z");

    ASSERT_TRUE(px != nullptr);
    ASSERT_TRUE(py != nullptr);
    ASSERT_TRUE(pz != nullptr);

    EXPECT_EQ("x", px->ident().toStdString());
    EXPECT_EQ("y", py->ident().toStdString());
    EXPECT_EQ("z", pz->ident().toStdString());

    // check values
    EXPECT_EQ(1.0, px->value<double>());
    EXPECT_EQ(2.0, py->value<double>());
    EXPECT_EQ(3.0, pz->value<double>());
}
