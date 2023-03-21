/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 13.3.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "gt_svgiconengine.h"
#include "gt_colors.h"

/// This is a test fixture that does a init for each test
struct TestGtSvgIconEngine : public ::testing::Test
{
    void toSvgColorData(gt::gui::SvgColorData data) { /* ... */ }
};

/// functions to test that SvgColorData can be constructed using a function
/// pointer and a Qt color enum value
TEST_F(TestGtSvgIconEngine, SvgColorData_implicit_conversion)
{
    using namespace gt::gui;

    toSvgColorData(color::basicDark);
    toSvgColorData(Qt::red);
    toSvgColorData(QColor{Qt::red});
    toSvgColorData(ColorFunctor{[](){ return Qt::red; }});
}

// test that this compiles
TEST_F(TestGtSvgIconEngine, SvgColorData_setters)
{
    using namespace gt::gui;

    auto data = SvgColorData{color::basicDark}
                .setSelectedColor(Qt::red)
                .setDisabledColor(QColor{Qt::blue})
                .setNormalColor([](){ return Qt::green; });
}
