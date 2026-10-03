/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_moduleinterface.h"

class FootprintTestModule : public QObject, public GtModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID GT_MODULE_ID FILE "metadata.json")
    GT_IMPLEMENT_MODULE_

public:
    GtVersionNumber version() override
    {
        return {1, 2, 3};
    }
    QString description() const override
    {
        return QStringLiteral("Test module");
    }
};

#include "module.moc"
