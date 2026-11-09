/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_processdockpage.h"

TEST(ProcessDockContributions, registerIndependently)
{
    const auto pageCount = gt::gui::processDockPageFactories().size();
    const auto addActionCount = gt::gui::processAddActions().size();
    const auto contextActionCount = gt::gui::processContextActions().size();

    gt::gui::registerProcessDockPage([] { return nullptr; });
    EXPECT_EQ(gt::gui::processDockPageFactories().size(), pageCount + 1);
    EXPECT_EQ(gt::gui::processAddActions().size(), addActionCount);
    EXPECT_EQ(gt::gui::processContextActions().size(), contextActionCount);

    gt::gui::registerProcessAddAction(
        GtObjectUIAction(QStringLiteral("Test Add"), [](GtObject*) {}));
    ASSERT_EQ(gt::gui::processAddActions().size(), addActionCount + 1);
    EXPECT_EQ(gt::gui::processAddActions().last().text(),
              QStringLiteral("Test Add"));
    EXPECT_EQ(gt::gui::processContextActions().size(), contextActionCount);

    gt::gui::registerProcessContextAction(
        GtObjectUIAction(QStringLiteral("Test Context"), [](GtObject*) {}));
    ASSERT_EQ(gt::gui::processContextActions().size(), contextActionCount + 1);
    EXPECT_EQ(gt::gui::processContextActions().last().text(),
              QStringLiteral("Test Context"));

    gt::gui::registerProcessDockPage({});
    gt::gui::registerProcessAddAction({});
    gt::gui::registerProcessContextAction({});
    EXPECT_EQ(gt::gui::processDockPageFactories().size(), pageCount + 1);
    EXPECT_EQ(gt::gui::processAddActions().size(), addActionCount + 1);
    EXPECT_EQ(gt::gui::processContextActions().size(), contextActionCount + 1);
}
