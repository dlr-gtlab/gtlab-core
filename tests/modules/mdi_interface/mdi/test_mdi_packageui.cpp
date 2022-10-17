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

#include "gt_dynamicinterface.h"

TestMdiPackageUI::TestMdiPackageUI()
{
    using namespace GtGUI;

    addSingleAction("Test Action", "testAction")
        .registerShortCut("testMdiShortCut", QKeySequence{"Alt+R"})
        .setIcon(Icon::config16());

    addActionGroup("Test Action Group", 3)
        .setIcon(Icon::folder16())
        << makeSingleAction("Test Group Action 1", "testGroupAction")
        << makeSingleAction("Test Group Action 2",
                            [](GtObject* /*obj*/){
              gtInfo() << "TEST MDI INTERFACE - TEST GROUP ACTION 2";
           })
        << makeSingleAction("Interface Function Test", "testDynamicInterface");
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
TestMdiPackageUI::testGroupAction(GtObject* /*obj*/)
{
    gtInfo() << "TEST MDI INTERFACE - TEST GROUP ACTION 1";
}

void
TestMdiPackageUI::testDynamicInterface(GtObject* /*obj*/)
{
    auto func = gtlab::interface::get_function("Test Datamodel Interface",
                                               "my_lambda_mult");

    QVariant result = -1.0;
    if (func.is_null())
    {
        gtError() << "Cannot find Interface function";
        return;
    }

    result = func({5, 7});

    gtInfo() << "The Result of the multiplication of 5 and 7 is" << result;
    gtInfo() << "The description of this function is" << func.help();

}

