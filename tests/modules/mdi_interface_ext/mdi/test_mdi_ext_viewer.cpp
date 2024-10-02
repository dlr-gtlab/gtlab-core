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

#include <gt_qmlaction.h>
#include <gt_logging.h>

#include <gt_icons.h>

TestMdiExtViewer::TestMdiExtViewer()
{
    using namespace gt::gui;

    cutAction = addToolbarAction("Cut", icon::url(icon::cut));
    copyAction  = addToolbarAction("Copy", icon::url(icon::copy));
    pasteAction = addToolbarAction("Paste", icon::url(icon::paste));

    pasteAction->setEnabled(false);

    connect(cutAction, &GtQmlAction::triggered, this, [this](){
        gtInfo() << "Cut";
        pasteAction->setEnabled(true);
    });

    connect(copyAction, &GtQmlAction::triggered, this, [this](){
        gtInfo() << "Copy";
        pasteAction->setEnabled(true);
    });

    connect(pasteAction, &GtQmlAction::triggered, this, [this](){
        gtInfo() << "Paste";
        pasteAction->setEnabled(false);
    });

    setObjectName("Test Mdi Ext Viewer");
}

bool
TestMdiExtViewer::allowsMultipleInstances() const
{
    return true;
}
