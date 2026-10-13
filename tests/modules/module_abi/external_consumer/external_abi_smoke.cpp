/* GTlab - Gas Turbine laboratory
 *
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 */

#include <gt_moduleinterface.h>

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
