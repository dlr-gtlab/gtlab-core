/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "test_gui_item_bound.h"

#include <QLabel>
#include <QVBoxLayout>

TestGuiItemBound::TestGuiItemBound()
{
    setObjectName("Bound Viewer");
    setItemFlag(BoundToObject, true);

    m_dataLabel = new QLabel{widget()};

    auto* lay = new QVBoxLayout{widget()};
    lay->addWidget(new QLabel{tr("BoundToObject flag set")});
    lay->addWidget(m_dataLabel);
    lay->addStretch();
}

void
TestGuiItemBound::setData(GtObject* obj)
{
    m_dataLabel->setText(obj ? QStringLiteral("data: %1").arg(obj->objectName())
                             : QStringLiteral("data: <none>"));

    if (obj)
    {
        setObjectName(
            QStringLiteral("Bound Viewer (%1)").arg(obj->objectName()));
    }
}
