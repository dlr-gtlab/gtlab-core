/* GTlab - Gas Turbine laboratory
 *
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 */

#include <gt_moduleinterface.h>
#include <QFile>

extern "C" int externalAbiDependencyValue();

namespace
{

struct ExternalModuleLoadMarker
{
    ExternalModuleLoadMarker()
    {
        const QString path =
            QString::fromLocal8Bit(qgetenv("GTLAB_ABI_TEST_MARKER"));
        if (path.isEmpty())
        {
            return;
        }

        QFile marker(path);
        if (marker.open(QIODevice::WriteOnly))
        {
            marker.write(QByteArray::number(externalAbiDependencyValue()));
        }
    }
};

ExternalModuleLoadMarker loadMarker;

} // namespace

// Minimal external module used to verify the installed GTlab package exports
// the ABI metadata and CMake integration required by downstream consumers.
class ExternalAbiSmoke : public QObject, public GtModuleInterface
{
    Q_OBJECT
    GT_MODULE()

public:
    GtVersionNumber version() override
    {
        return {1, 0, 0};
    }

    QString description() const override
    {
        return QStringLiteral("External module package smoke test");
    }
};
