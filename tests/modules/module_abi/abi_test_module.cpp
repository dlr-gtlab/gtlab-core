/* GTlab - Gas Turbine laboratory
 *
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 */

#include "abi_test_module.h"

#include <QFile>

extern "C" int abiAdjacentDependencyValue();

namespace
{

    struct LoadMarker
    {
        LoadMarker()
        {
            const QString path =
                QString::fromLocal8Bit(qgetenv("GTLAB_ABI_TEST_MARKER"));
            if (!path.isEmpty())
            {
                QFile marker(path);
                if (marker.open(QIODevice::WriteOnly))
                {
                    marker.write(
                        QByteArray::number(abiAdjacentDependencyValue()));
                }
            }
        }
    };

    LoadMarker loadMarker;

} // namespace

GtVersionNumber
AbiTestModule::version()
{
    return {1, 0, 0};
}

QString
AbiTestModule::description() const
{
    return QStringLiteral("ABI compatibility test module");
}
