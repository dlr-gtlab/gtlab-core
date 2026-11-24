/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "test_gui_item_customid.h"

#include <QLabel>
#include <QVBoxLayout>

TestGuiItemCustomId::TestGuiItemCustomId()
{
    setObjectName("Shared Viewer");

    m_dataLabel = new QLabel{widget()};

    auto* lay = new QVBoxLayout{widget()};
    lay->addWidget(new QLabel{tr("Constant Custom ID, no flags set")});
    lay->addWidget(m_dataLabel);
    lay->addStretch();
}

QString
TestGuiItemCustomId::customId(GtObject* /*obj*/)
{
    return QStringLiteral("shared");
}

void
TestGuiItemCustomId::setData(GtObject* obj)
{
    m_dataLabel->setText(obj ? QStringLiteral("data: %1").arg(obj->objectName())
                             : QStringLiteral("data: <none>"));

    if (obj)
    {
        setObjectName(
            QStringLiteral("Shared Viewer (%1)").arg(obj->objectName()));
    }
}
