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

#include "test_mdi_ext_packageui.h"
#include "test_mdi_ext_viewer.h"

TestMdiExtPackageUI::TestMdiExtPackageUI()
{
    addSingleAction("Test Action 2", "testAction");

    QList<GtObjectUIAction> addItemActions;

    addItemActions << GtObjectUIAction("Test Group Action",
                                       "testGroupAction");

    addActionGroup("Test Action Group 2", addItemActions);
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
