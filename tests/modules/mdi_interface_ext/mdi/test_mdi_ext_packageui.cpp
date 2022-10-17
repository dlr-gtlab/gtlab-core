/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_ext_packageui.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 15.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_logging.h"
#include "gt_application.h"

#include "gt_package.h"
#include "test_mdi_ext_packageui.h"
#include "test_mdi_ext_viewer.h"

#include "gt_icons.h"

TestMdiExtPackageUI::TestMdiExtPackageUI()
{

    auto k = registerShortCut(QStringLiteral("testMdiExtShortCut_1"),
                              QKeySequence(Qt::Key_F9));

    addSingleAction("Test Action 2", "testAction")
        .setShortCut(k);

    addActionGroup("Test Action Group 2")
        .setIcon("folder_16.png")
        << makeSingleAction("Test Group Action", "testGroupAction")
            .registerShortCut("testMdiExtShortCut_2", "testMdiExt",
                              QKeySequence(Qt::Key_F10), true);
}

QIcon
TestMdiExtPackageUI::icon(GtObject* /*obj*/) const
{
    return QIcon();
}

QStringList
TestMdiExtPackageUI::openWith(GtObject* /*obj*/)
{
    QStringList retval;

    retval << GT_CLASSNAME(TestMdiExtViewer);

    return retval;
}

void
TestMdiExtPackageUI::doubleClicked(GtObject* /*obj*/)
{
    gtInfo() << "TEST MDI INTERFACE EXT - DOUBLE CLICKED";
}

void
TestMdiExtPackageUI::testAction(GtObject* /*obj*/)
{
    gtInfo() << "TEST MDI INTERFACE EXT - TEST ACTION";
}

void
TestMdiExtPackageUI::testGroupAction(GtObject* /*obj*/)
{
    gtInfo() << "TEST MDI INTERFACE EXT - TEST GROUP ACTION";
}

