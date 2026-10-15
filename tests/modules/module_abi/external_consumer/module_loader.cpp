/* GTlab - Gas Turbine laboratory
 *
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 */

#include <QCoreApplication>
#include <QDebug>
#include <QLibrary>

int
main(int argc, char* argv[])
{
    QCoreApplication application(argc, argv);
    if (argc != 2)
    {
        qCritical() << "Expected the path to a module";
        return 2;
    }

    QLibrary module(QString::fromLocal8Bit(argv[1]));
    if (!module.load())
    {
        qCritical() << "Failed to load installed module:"
                    << module.errorString();
        return 1;
    }

    return 0;
}
