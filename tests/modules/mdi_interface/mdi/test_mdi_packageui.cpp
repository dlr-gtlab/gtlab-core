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
    addSingleAction("Test Action", "testAction")
        .registerShortCut("testMdiShortCut", QKeySequence{"Alt+R"})
        .setIcon(gt::gui::icon::config16());

    addActionGroup("Test Action Group", 3)
        .setIcon(gt::gui::icon::folder16())
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
    return gt::gui::icon::global();
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
    auto func = gt::interface::getFunction("Test Datamodel Interface",
                                            "my_lambda_mult");

    QVariant result = -1.0;
    if (func.isNull())
    {
        gtError() << "Cannot find Interface function";
        return;
    }

    result = func({5, 7});

    gtInfo() << "The Result of the multiplication of 5 and 7 is" << result;
    gtInfo() << "The description of this function is" << func.help();

}

