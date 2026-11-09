/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include <QCoreApplication>
#include <QScopedPointer>
#include <QDir>
#include <QTextStream>
#include <QThread>

#include "gt_applicationpaths.h"
#include "gt_testhelper.h"
#include "gt_externalizationmanager.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    const QStringList arguments = app.arguments();
    if (arguments.size() > 1 &&
        arguments.at(1).startsWith(QStringLiteral("--application-temp-dir-")))
    {
        QCoreApplication::setOrganizationName(QStringLiteral("DLR"));
        QCoreApplication::setApplicationName(QStringLiteral("GTlab 2"));

        const auto directory =
            GtApplicationPaths::directory(GtApplicationPaths::Location::Temp);
        if (!directory)
        {
            return 2;
        }

        QTextStream(stdout)
            << "GTLAB_TEMP_PATH=" << directory->absolutePath() << Qt::endl;
        if (arguments.at(1) == QStringLiteral("--application-temp-dir-hold"))
        {
            while (true)
            {
                QThread::sleep(1);
            }
        }

        return 0;
    }

    QScopedPointer<GtTestHelper> helper {GtTestHelper::instance()};

    // set custom path for externalization
    gtExternalizationManager->onProjectLoaded(
                helper->newTempDir().absolutePath());

    ::testing::InitGoogleTest(&argc, argv);
    int retval = RUN_ALL_TESTS();

    return retval;
}
