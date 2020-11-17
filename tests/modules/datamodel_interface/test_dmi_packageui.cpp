/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_packageui.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 15.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_logging.h"

#include "test_dmi_packageui.h"

TestDmiPackageUI::TestDmiPackageUI()
{
    addSingleAction("Test Action", "testAction");

    QList<GtObjectUIAction> addItemActions;

    addItemActions << GtObjectUIAction("Test Group Action 1",
                                       "testGroupAction1");

    addItemActions << GtObjectUIAction("Test Group Action 2",
                                       "testGroupAction2");

    addActionGroup("Test Action Group", addItemActions);
}

QStringList
TestDmiPackageUI::openWith(GtObject* /*obj*/)
{
    QStringList retval;

    retval << "Opener 1" << "Opener 2";

    return retval;
}

void
TestDmiPackageUI::testAction(GtObject* /*obj*/)
{
    gtInfo() << "TEST DATA MODEL INTERFACE - TEST ACTION";
}

void
TestDmiPackageUI::testGroupAction1(GtObject* /*obj*/)
{

}

void
TestDmiPackageUI::testGroupAction2(GtObject* /*obj*/)
{

}
