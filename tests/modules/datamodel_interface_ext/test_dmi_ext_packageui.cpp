/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_ext_packageui.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 15.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_logging.h"

#include "test_dmi_ext_packageui.h"

TestDmiExtPackageUI::TestDmiExtPackageUI()
{
    addSingleAction("Test Action 2", "testAction");

    QList<GtObjectUIAction> addItemActions;

    addItemActions << GtObjectUIAction("Test Group Action",
                                       "testGroupAction");

    addActionGroup("Test Action Group 2", addItemActions);
}

QStringList
TestDmiExtPackageUI::openWith(GtObject* /*obj*/)
{
    QStringList retval;

    retval << "Opener Ext 1" << "Opener Ext 2";

    return retval;
}

void
TestDmiExtPackageUI::testAction(GtObject* /*obj*/)
{
    gtInfo() << "TEST DATA MODEL INTERFACE EXT - TEST ACTION";
}

void
TestDmiExtPackageUI::testGroupAction(GtObject* /*obj*/)
{

}
