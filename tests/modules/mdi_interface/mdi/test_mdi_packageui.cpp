/* GTlab - Gas Turbine laboratory
 * Source File: test_mdi_packageui
 * copyright 2009-2020 by DLR
 *
 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#include <QIcon>

#include "gt_logging.h"
#include "gt_icons.h"

#include "test_mdi_packageui.h"
#include "test_mdi_viewer.h"

#include "gt_dynamicinterfacehandler.h"

TestMdiPackageUI::TestMdiPackageUI()
{
    addSingleAction("Test Action", "testAction");

    QList<GtObjectUIAction> addItemActions;

    addItemActions << GtObjectUIAction("Test Group Action 1",
                                       "testGroupAction1");

    addItemActions << GtObjectUIAction("Test Group Action 2",
                                       "testGroupAction2");

    addItemActions << GtObjectUIAction("Interface Function Test",
                                       "testGroupAction3");

    addActionGroup("Test Action Group", addItemActions);

}

QIcon
TestMdiPackageUI::icon(GtObject* /*obj*/) const
{
    return GtGUI::Icon::global();
}

QStringList
TestMdiPackageUI::openWith(GtObject* /*obj*/)
{
    QStringList retval;

    retval << GT_CLASSNAME(TestMdiViewer);

    return retval;
}

void
TestMdiPackageUI::doubleClicked(GtObject* obj)
{
    gtInfo() << "TEST MDI INTERFACE - DOUBLE CLICKED";

    GtObjectUI::doubleClicked(obj);
}

void
TestMdiPackageUI::testAction(GtObject* /*obj*/)
{
    gtInfo() << "TEST MDI INTERFACE - TEST ACTION";
}

void
TestMdiPackageUI::testGroupAction1(GtObject* /*obj*/)
{
    gtInfo() << "TEST MDI INTERFACE - TEST GROUP ACTION 1";
}

void
TestMdiPackageUI::testGroupAction2(GtObject* /*obj*/)
{
    gtInfo() << "TEST MDI INTERFACE - TEST GROUP ACTION 2";
}

void
TestMdiPackageUI::testGroupAction3(GtObject* /*obj*/)
{
    auto func = gtlab::interface::get_function("testdmmod", "my_lambda_mult");

    QVariant result = -1.0;
    if (!func.is_null())
    {
        result = func({5, 7});
    }
    else
    {
        gtError() << "Cannot find Interface function";
    }

    gtInfo() << "The Result of the multiplication of 5 nd 7 is " << result;
    gtInfo() << "The description of this function is" << func.help();

}

