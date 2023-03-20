/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 21.2.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "test_mdi_ext_externalobjectui.h"
#include "test_mdi_ext_externalobjectviewer.h"
#include "test_dmi_externalobject.h"

#include "gt_icons.h"
#include "gt_logging.h"
#include "gt_utilities.h"
#include "gt_datamodel.h"
#include "gt_application.h"
#include "gt_command.h"
#include "gt_objectfactory.h"

#include <cmath>

TestMdiExtExternalObjectUI::TestMdiExtExternalObjectUI()
{
    addSingleAction(tr("Create sibling"), createSibling)
            .setIcon(gt::gui::icon::add());
    addSingleAction(tr("Modify values"), modifyValues)
            .setIcon(gt::gui::icon::calculator());
    addSingleAction(tr("Modify params"), modifyParams)
            .setIcon(gt::gui::icon::calculator());
    addSingleAction(tr("Append data"), addData)
            .setIcon(gt::gui::icon::mathPlus());
    addSingleAction(tr("Info"), [](GtObject* obj){
        if (auto* extObj = qobject_cast<TestDmiExternalObject*>(obj))
        {
            gtInfo() << "Reference Count:" << extObj->refCount();
        }
    }).setIcon(gt::gui::icon::info2());
}

QIcon
TestMdiExtExternalObjectUI::icon(GtObject* /*obj*/) const
{
    return gt::gui::icon::data();
}

void
TestMdiExtExternalObjectUI::createSibling(GtObject* obj)
{
    if (!obj) return;

    auto* parent = obj->parentObject();
    if (!parent) return;

    auto sibling = std::make_unique<TestDmiExternalObject>();

    gtDebug() << sibling;

    sibling->setObjectName(
                gtDataModel->uniqueObjectName(sibling->objectName(), parent));

    if (!gtDataModel->appendChild(sibling.get(), parent).isValid())
    {
        gtError() << tr("Failed to add silbling!");
        return;
    }
    sibling.release();
}

void
TestMdiExtExternalObjectUI::modifyValues(GtObject* obj)
{
    auto* extObj = qobject_cast<TestDmiExternalObject*>(obj);
    if (!extObj) return;

    auto cmd = gtApp->startCommand(extObj->parentObject(), "Test::modifyValues");
    auto finally = gt::finally([&](){
        gtApp->endCommand(cmd);
    });

    auto data = extObj->fetchData();
    auto vals = data.values();
    std::reverse(std::begin(vals), std::end(vals));
    data.setValues(std::move(vals));
}

void
TestMdiExtExternalObjectUI::modifyParams(GtObject* obj)
{
    auto* extObj = qobject_cast<TestDmiExternalObject*>(obj);
    if (!extObj) return;

    auto cmd = gtApp->startCommand(extObj->parentObject(), "Test::modifyParams");
    auto finally = gt::finally([&](){
        gtApp->endCommand(cmd);
    });

    auto data = extObj->fetchData();
    auto params = data.params();
    std::reverse(std::begin(params), std::end(params));
    data.setParams(std::move(params));
}

void
TestMdiExtExternalObjectUI::addData(GtObject* obj)
{
    auto* extObj = qobject_cast<TestDmiExternalObject*>(obj);
    if (!extObj) return;

    auto data = extObj->fetchData();

    auto cmd = gtApp->startCommand(extObj->parentObject(), "Test::addData");
    auto finally = gt::finally([&](){
        gtApp->endCommand(cmd);
    });

    auto params = data.params();
    auto values = data.values();

    // add single entry
    params.append(QStringLiteral("param_") + QString::number(params.size()));
    double size = values.size();
    double value = size;
    for (int i = 0; i < 3; ++i)
    {
        value += (size + i + 1) * std::pow(0.1, i + 1);
    }
    values.append(value);

    data.setValues(std::move(values));
    data.setParams(std::move(params));
}

QStringList
TestMdiExtExternalObjectUI::openWith(GtObject* obj)
{
    auto* extObj = qobject_cast<TestDmiExternalObject*>(obj);
    if (!extObj) return {};

    return { GT_CLASSNAME(TestMdiExtExternalObjectViewer) };
}
