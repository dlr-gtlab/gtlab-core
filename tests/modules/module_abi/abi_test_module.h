/* GTlab - Gas Turbine laboratory
 *
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 */

#pragma once

#include "gt_moduleinterface.h"

/// Shared fixture implementation for compatible, incompatible, and
/// metadata-free module ABI test variants.
class AbiTestModule : public QObject, public GtModuleInterface
{
    Q_OBJECT
    GT_MODULE()

public:
    GtVersionNumber version() override;
    QString description() const override;
};
