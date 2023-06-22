/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 21.2.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "test_mdi_ext_viewer.h"

TestMdiExtViewer::TestMdiExtViewer()
{
    setObjectName("Test Mdi Ext Viewer");
}

bool
TestMdiExtViewer::allowsMultipleInstances() const
{
    return true;
}
