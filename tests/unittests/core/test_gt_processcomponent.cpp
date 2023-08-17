/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_processcomponent.cpp
 * copyright 2009-2019 by DLR
 *
 *  Created on: 17.07.2023
 *  Author: Marvin Noethen (AT-TW)
 *  Tel.: +49 2203 601 2692
 */
#include "gtest/gtest.h"

#include "test_gt_processtestclasses.h"

#include "gt_objectfactory.h"

class TestGtProcessComponentFunctions : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        processComp.setFactory(gtObjectFactory);

        gtObjectFactory->registerClass(
                    TestGtProcessComponent::staticMetaObject);
    }

    virtual void TearDown()
    {

    }

    TestGtProcessComponent processComp;
};

TEST_F(TestGtProcessComponentFunctions, findMonitoringDynamicSizeProperty)
{
    ASSERT_TRUE(processComp.findPropertyContainer("monitoringVars"));
}

TEST_F(TestGtProcessComponentFunctions, checkContainerMonitoringPropSize)
{
    const auto& monProps = processComp.monitoringProperties();
    const auto& containerMonPropRefs =
            processComp.containerMonitoringPropertyRefs();

    EXPECT_EQ(0u, monProps.size());
    EXPECT_EQ(0u, containerMonPropRefs.size());

    auto* props = processComp.findPropertyContainer("monitoringVars");
    ASSERT_TRUE(props);

    EXPECT_EQ(0u, props->size());

    processComp.addMonitoringVar("name", 42);

    EXPECT_EQ(1u, props->size());

    EXPECT_EQ(0u, monProps.size());
    EXPECT_EQ(1u, containerMonPropRefs.size());

    props->removeEntry(props->begin());

    EXPECT_EQ(0u, containerMonPropRefs.size());
}
