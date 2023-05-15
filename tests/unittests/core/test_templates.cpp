/* GTlab - Gas Turbine laboratory
 * copyright 2023 by DLR
 *
 *  Created on: 09.05.2023
 *  Author: Martin Siggel (AT-TWK)
 */

#include "gtest/gtest.h"

#include <QFile>

TEST(TestTemplates, templateExists)
{
    QFile templateFile(":/templates/empty_project.xml");

    EXPECT_TRUE(templateFile.exists());
}
