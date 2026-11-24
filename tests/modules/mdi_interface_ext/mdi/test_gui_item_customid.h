/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TESTGUIITEMCUSTOMID_H
#define TESTGUIITEMCUSTOMID_H

#include "gt_mdiitem.h"

class QLabel;

/**
 * @brief GUI item that collapses all data objects into a single shared tab
 * via a constant `customId`, but does NOT reopen (no `AllowsReopening`).
 */
class TestGuiItemCustomId : public GtMdiItem
{
    Q_OBJECT

public:
    Q_INVOKABLE TestGuiItemCustomId();

    void setData(GtObject* obj) override;

protected:
    QString customId(GtObject* obj) override;

private:
    QLabel* m_dataLabel{nullptr};
};

#endif // TESTGUIITEMCUSTOMID_H
