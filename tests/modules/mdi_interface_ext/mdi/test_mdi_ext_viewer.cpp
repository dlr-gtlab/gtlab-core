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

    auto toggleAction = addToolbarAction("View", icon::eyeOff_Url());
    toggleAction->setCheckable(true);

    connect(toggleAction, &GtQmlAction::toggled, this, [toggleAction](bool checked){
        if (checked) {
            toggleAction->setIconUrl(icon::eye_Url());
        }
        else {
            toggleAction->setIconUrl(icon::eyeOff_Url());
        }
    });

    cutAction = addToolbarAction("Cut", icon::cut_Url());
    copyAction  = addToolbarAction("Copy", icon::copy_Url());

    addToolbarSeparator();

    pasteAction = addToolbarAction("Paste", icon::paste_Url());

    pasteAction->setVisible(false);

    connect(cutAction, &GtQmlAction::triggered, this, [this](){
        gtInfo() << "Cut";
        pasteAction->setEnabled(!pasteAction->enabled());
    });

    connect(copyAction, &GtQmlAction::triggered, this, [this](){
        gtInfo() << "Copy";
        pasteAction->setVisible(true);
    });

    connect(pasteAction, &GtQmlAction::triggered, this, [this](){
        gtInfo() << "Paste";
        pasteAction->setVisible(false);
    });

    setObjectName("Test Mdi Ext Viewer");
}

bool
TestMdiExtViewer::allowsMultipleInstances() const
{
    return true;
}
