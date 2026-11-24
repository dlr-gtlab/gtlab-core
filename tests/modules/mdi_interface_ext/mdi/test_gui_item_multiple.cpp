/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "test_gui_item_multiple.h"

#include <QLabel>
#include <QVBoxLayout>

TestGuiItemMultiple::TestGuiItemMultiple()
{
    setObjectName("Multi Item Viewer");
    setItemFlag(AllowsMultipleInstances, true);

    m_dataLabel = new QLabel{widget()};

    auto* lay = new QVBoxLayout{widget()};
    lay->addWidget(new QLabel{tr("AllowsMultipleInstances flag set")});
    lay->addWidget(m_dataLabel);
    lay->addStretch();
}

void
TestGuiItemMultiple::setData(GtObject* obj)
{
    m_dataLabel->setText(obj ? QStringLiteral("data: %1").arg(obj->objectName())
                             : QStringLiteral("data: <none>"));

    if (obj)
    {
        setObjectName(
            QStringLiteral("Multi Item Viewer (%1)").arg(obj->objectName()));
    }
}
