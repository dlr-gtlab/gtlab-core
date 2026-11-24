/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "test_gui_item_nodata.h"

#include <QLabel>
#include <QVBoxLayout>

TestGuiItemNoData::TestGuiItemNoData()
{
    setObjectName("Free Viewer");

    auto* lay = new QVBoxLayout{widget()};
    lay->addWidget(new QLabel{tr("No data object required")});
    lay->addStretch();
}
