/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TESTGUIITEMNODATA_H
#define TESTGUIITEMNODATA_H

#include "gt_mdiitem.h"

/**
 * @brief GUI item that does not operate on a data object. It is intended to be
 * opened without a `GtObject`.
 */
class TestGuiItemNoData : public GtMdiItem
{
    Q_OBJECT

public:
    Q_INVOKABLE TestGuiItemNoData();
};

#endif // TESTGUIITEMNODATA_H
