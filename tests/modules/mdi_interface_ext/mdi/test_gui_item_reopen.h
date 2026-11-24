/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef TESTGUIITEMREOPEN_H
#define TESTGUIITEMREOPEN_H

#include "gt_mdiitem.h"

class QLabel;

/**
 * @brief GUI item that reuses a single tab for any data object
 * (`AllowsReopening` flag plus a constant `customId`).
 */
class TestGuiItemReopen : public GtMdiItem
{
    Q_OBJECT

public:
    Q_INVOKABLE TestGuiItemReopen();

    void setData(GtObject* obj) override;

protected:
    QString customId(GtObject* obj) override;

private:
    QLabel* m_dataLabel{nullptr};
};

#endif // TESTGUIITEMREOPEN_H
