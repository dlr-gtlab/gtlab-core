/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_modeproperty.cpp
 *
 *  Created on: 03.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include <QString>

#include "gt_modeproperty.h"
#include "gt_modetypeproperty.h"

/// This is a test fixture that does a init for each test
class TestGtModeProperty : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_prop = new GtModeProperty("prop", "test mode", "test brief");
        m_prop2 = new GtModeProperty("prop", "test mode", "test brief");
        m_prop3 = new GtModeProperty("prop", "test mode", "test brief");

        m_propTypeA = new GtModeTypeProperty("Type A", "Type A brief");
        m_propTypeB = new GtModeTypeProperty("Type B", "Type B brief");
        m_propTypeC = new GtModeTypeProperty("Type C", "Type C brief");

        m_prop2->registerSubProperty(*m_propTypeA);
        m_prop3->registerSubProperty(*m_propTypeB);
        m_prop3->registerSubProperty(*m_propTypeC);
    }

    virtual void TearDown()
    {
        delete m_prop;
        delete m_prop2;
        delete m_prop3;
        delete m_propTypeA;
        delete m_propTypeB;
        delete m_propTypeC;
    }

    GtModeProperty* m_prop;
    GtModeProperty* m_prop2;
    GtModeProperty* m_prop3;

    GtModeTypeProperty* m_propTypeA;
    GtModeTypeProperty* m_propTypeB;
    GtModeTypeProperty* m_propTypeC;

};

TEST_F(TestGtModeProperty, initialization)
{
    // default constructor
    ASSERT_STREQ(m_prop->get().toStdString().c_str(), "");
    ASSERT_STREQ(m_prop->get().toStdString().c_str(), "");
    ASSERT_EQ(m_prop->unitCategory(), GtUnit::None);
    ASSERT_STREQ(m_prop->objectName().toStdString().c_str(),
                 "test mode");
    ASSERT_STREQ(m_prop->brief().toStdString().c_str(),
                 "test brief");

    // mode type properties
    ASSERT_STREQ(m_propTypeA->get().toStdString().c_str(), "Type A");
    ASSERT_EQ(m_propTypeA->unitCategory(), GtUnit::None);
    ASSERT_STREQ(m_propTypeA->objectName().toStdString().c_str(),
                 "Type A");
    ASSERT_STREQ(m_propTypeA->brief().toStdString().c_str(),
                 "Type A brief");

    ASSERT_STREQ(m_propTypeB->get().toStdString().c_str(), "Type B");
    ASSERT_EQ(m_propTypeB->unitCategory(), GtUnit::None);
    ASSERT_STREQ(m_propTypeB->objectName().toStdString().c_str(),
                 "Type B");
    ASSERT_STREQ(m_propTypeB->brief().toStdString().c_str(),
                 "Type B brief");

    ASSERT_STREQ(m_propTypeC->get().toStdString().c_str(), "Type C");
    ASSERT_EQ(m_propTypeC->unitCategory(), GtUnit::None);
    ASSERT_STREQ(m_propTypeC->objectName().toStdString().c_str(),
                 "Type C");
    ASSERT_STREQ(m_propTypeC->brief().toStdString().c_str(),
                 "Type C brief");

    // single type
    ASSERT_STREQ(m_prop2->get().toStdString().c_str(), "Type A");
    ASSERT_EQ(m_prop2->unitCategory(), GtUnit::None);
    ASSERT_STREQ(m_prop2->objectName().toStdString().c_str(),
                 "test mode");
    ASSERT_STREQ(m_prop2->brief().toStdString().c_str(),
                 "test brief");

    // multiple types
    ASSERT_STREQ(m_prop3->get().toStdString().c_str(), "Type B");
    ASSERT_EQ(m_prop3->unitCategory(), GtUnit::None);
    ASSERT_STREQ(m_prop3->objectName().toStdString().c_str(),
                 "test mode");
    ASSERT_STREQ(m_prop3->brief().toStdString().c_str(),
                 "test brief");
}

TEST_F(TestGtModeProperty, compare)
{
    ASSERT_STREQ(m_prop2->get().toStdString().c_str(),
                 m_propTypeA->get().toStdString().c_str());

    QString modeVal = *m_prop2;
    QString modeTypeVal = *m_propTypeA;

    ASSERT_STREQ(modeVal.toStdString().c_str(), "Type A");
    ASSERT_STREQ(modeTypeVal.toStdString().c_str(), "Type A");

    ASSERT_STREQ(modeVal.toStdString().c_str(),
                 modeTypeVal.toStdString().c_str());
}
