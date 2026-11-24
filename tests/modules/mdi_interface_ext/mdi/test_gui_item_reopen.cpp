/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "test_gui_item_reopen.h"

#include <QLabel>
#include <QVBoxLayout>

TestGuiItemReopen::TestGuiItemReopen()
{
    setObjectName("Reopen Viewer");
    setItemFlag(AllowsReopening, true);

    m_dataLabel = new QLabel{widget()};

    auto* lay = new QVBoxLayout{widget()};
    lay->addWidget(
        new QLabel{tr("Constant Custom ID, AllowsReopening flag set")});
    lay->addWidget(m_dataLabel);
    lay->addStretch();
}

QString
TestGuiItemReopen::customId(GtObject* /*obj*/)
{
    // same id for every data object -> only one tab, reopened with new data
    return QStringLiteral("shared");
}

void
TestGuiItemReopen::setData(GtObject* obj)
{
    m_dataLabel->setText(obj ? QStringLiteral("data: %1").arg(obj->objectName())
                             : QStringLiteral("data: <none>"));

    if (obj)
    {
        setObjectName(
            QStringLiteral("Reopen Viewer (%1)").arg(obj->objectName()));
    }
}
