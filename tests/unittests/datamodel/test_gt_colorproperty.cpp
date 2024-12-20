/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 * Source File: test_gt_colorproperty
 *
 *  Created on: 12.12.2024
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gtest/gtest.h"

#include "gt_colorproperty.h"
#include <QColor>

/// This is a test fixture that does a init for each test
class TestGtColorProperty : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        gt::rgb rgb(255, 0, 0); // red
        m_prop = new GtColorProperty("prop", "test color", "test brief", rgb);
    }

    virtual void TearDown()
    {
        delete m_prop;
    }

    GtColorProperty* m_prop;

};

TEST_F(TestGtColorProperty, initialization)
{
    // default constructor
    EXPECT_EQ(m_prop->unitCategory(), GtUnit::None);
    EXPECT_STREQ(m_prop->objectName().toStdString().c_str(),
                 "test color");
    EXPECT_STREQ(m_prop->brief().toStdString().c_str(),
                 "test brief");
}

TEST_F(TestGtColorProperty, isReadOnly)
{
    m_prop->setReadOnly(true);
    EXPECT_TRUE(m_prop->isReadOnly());
}

TEST_F(TestGtColorProperty, getter)
{
    std::string compare = "#FF0000";

    gt::rgb rgb = m_prop->toRGB();

    EXPECT_STREQ(m_prop->hexString().toStdString().c_str(), compare.c_str());

    QColor fromProp = QColor(m_prop->getVal());
    EXPECT_TRUE(fromProp == QColor(Qt::red));
}

TEST_F(TestGtColorProperty, setter)
{
    m_prop->setVal(QColor(Qt::blue).name());
    EXPECT_STREQ(m_prop->hexString().toStdString().c_str(), "#0000FF");
}

TEST_F(TestGtColorProperty, toRGB)
{
    m_prop->setVal(QColor(Qt::blue).name());
    EXPECT_STREQ(m_prop->hexString().toStdString().c_str(), "#0000FF");
    gt::rgb test1 = m_prop->toRGB();
    EXPECT_EQ(test1.m_r, uint8_t(0));
    EXPECT_EQ(test1.m_g, uint8_t(0));
    EXPECT_EQ(test1.m_b, uint8_t(255));

    m_prop->setVal(QColor(Qt::yellow).name());
    gt::rgb test2 = m_prop->toRGB();
    EXPECT_EQ(test2.m_r, uint8_t(255));
    EXPECT_EQ(test2.m_g, uint8_t(255));
    EXPECT_EQ(test2.m_b, uint8_t(0));
}

TEST_F(TestGtColorProperty, toRGBandAlpha)
{
    m_prop->setFromRGB(gt::rgb(0, 0, 255, 255));
    EXPECT_STREQ(m_prop->hexString().toStdString().c_str(), "#0000FF");
    gt::rgb test1 = m_prop->toRGB();
    EXPECT_EQ(test1.m_r, uint8_t(0));
    EXPECT_EQ(test1.m_g, uint8_t(0));
    EXPECT_EQ(test1.m_b, uint8_t(255));
    EXPECT_EQ(test1.m_alpha, uint8_t(255));

    m_prop->setVal(gt::rgb(255, 255, 0, 0).toHexString());
    gt::rgb test2 = m_prop->toRGB();
    EXPECT_EQ(test2.m_r, uint8_t(255));
    EXPECT_EQ(test2.m_g, uint8_t(255));
    EXPECT_EQ(test2.m_b, uint8_t(0));
    EXPECT_EQ(test2.m_alpha, uint8_t(0));
}

TEST_F(TestGtColorProperty, setFromRGB)
{
    gt::rgb orange(255, 165, 0);
    std::string orangeHex = "#FFA500";

    m_prop->setFromRGB(orange);
    EXPECT_STREQ(m_prop->hexString().toStdString().c_str(), orangeHex.c_str());

    gt::rgb darkYellow(Qt::darkYellow);
    darkYellow.m_alpha = 0;
    std::string compare = "#00808000";

    m_prop->setFromRGB(darkYellow);
    EXPECT_STREQ(m_prop->hexString().toStdString().c_str(), compare.c_str());
}

