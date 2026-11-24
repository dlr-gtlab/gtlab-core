/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TESTGUIITEMMULTIPLE_H
#define TESTGUIITEMMULTIPLE_H

#include "gt_mdiitem.h"

class QLabel;

/**
 * @brief GUI item that enables multiple instances via the
 * `AllowsMultipleInstances` flag (instead of overriding the virtual method).
 */
class TestGuiItemMultiple : public GtMdiItem
{
    Q_OBJECT

public:
    Q_INVOKABLE TestGuiItemMultiple();

    void setData(GtObject* obj) override;

private:
    QLabel* m_dataLabel{nullptr};
};

#endif // TESTGUIITEMMULTIPLE_H
