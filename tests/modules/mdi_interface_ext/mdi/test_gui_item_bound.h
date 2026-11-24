/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TESTGUIITEMBOUND_H
#define TESTGUIITEMBOUND_H

#include "gt_mdiitem.h"

class QLabel;

/**
 * @brief GUI item bound to its data object via the `BoundToObject` flag:
 * refuses to open without an object and is deleted when the object is deleted.
 */
class TestGuiItemBound : public GtMdiItem
{
    Q_OBJECT

public:
    Q_INVOKABLE TestGuiItemBound();

    void setData(GtObject* obj) override;

private:
    QLabel* m_dataLabel{nullptr};
};

#endif // TESTGUIITEMBOUND_H
