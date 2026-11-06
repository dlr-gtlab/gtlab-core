/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <QApplication>
#include <QScopedPointer>
#include <QDir>

#include "gt_testhelper.h"
#include "gt_externalizationmanager.h"

int main(int argc, char **argv)
{
    // GUI component tests run headlessly unless a platform was requested.
    if (qgetenv("QT_QPA_PLATFORM").isEmpty())
    {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }
    QApplication app(argc, argv);

    QScopedPointer<GtTestHelper> helper {GtTestHelper::instance()};

    // set custom path for externalization
    gtExternalizationManager->onProjectLoaded(
                helper->newTempDir().absolutePath());

    ::testing::InitGoogleTest(&argc, argv);
    int retval = RUN_ALL_TESTS();

    return retval;
}
