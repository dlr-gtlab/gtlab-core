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

TestMdiExtViewer::TestMdiExtViewer()
    : cutAction(new GtQmlAction("Cut", QUrl("/icons/cut.svg"), this))
    , copyAction(new GtQmlAction("Copy", QUrl("/icons/copy.svg"), this))
    , pasteAction(new GtQmlAction("Paste", QUrl("/icons/paste.svg"), this))

{
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

std::vector<GtQmlAction *>
TestMdiExtViewer::toolbarActions() const
{
    return {cutAction, copyAction, pasteAction};
}
