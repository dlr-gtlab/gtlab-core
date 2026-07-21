/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <gt_icons.h>

TEST(Icons, name)
{
    EXPECT_EQ(":/icons/error.svg",
              gt::gui::icon::error().path().toStdString());
}
