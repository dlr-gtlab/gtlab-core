/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_datamodel_interface
 *
 * Created on: 27.05.2020
 * Author: S. Reitenbach
 */

#include "test_dmi_package.h"
#include "test_dmi_data.h"
#include "test_dmi_externalobject.h"

#include "gt_functionalinterface.h"
#include "gt_propertyconversionregistry.h"
#include "gt_intproperty.h"
#include "gt_boolproperty.h"

/**
 * generated (v 0.1)
 */

#include "test_datamodel_interface.h"

void
TestDatamodelInterface::init()
{
    auto& registry = gtPropConversion();

    registry.registerConnectionCompatibility(
        GtBoolProperty::staticMetaObject,
        GtIntProperty::staticMetaObject,
        [](GtAbstractProperty const& a,
           GtAbstractProperty& b)
        {
            const auto& from = static_cast<const GtBoolProperty&>(a);
            auto& to = static_cast<GtIntProperty&>(b);

            if (from.get()) to.setVal(1);
            else to.setVal(0);

            return gt::conversion::conversionSuccess::Success;
        },
        [](GtAbstractProperty const&,
           GtAbstractProperty const&)
        {
            return true;
        });
}

GtVersionNumber
TestDatamodelInterface::version()
{
    return GtVersionNumber(1);
}

QString
TestDatamodelInterface::description() const
{
    return "Test Datamodel Interface";
}

///gtdatamodelinterface
///generated

QList<QMetaObject>
TestDatamodelInterface::data()
{
    QList<QMetaObject> metaData;

    metaData << GT_METADATA(TestDmiData);
    metaData << GT_METADATA(TestDmiExternalObject);

    return metaData;
}

QMetaObject
TestDatamodelInterface::package()
{
    return GT_METADATA(TestDmiPackage);
}

bool
TestDatamodelInterface::standAlone()
{
    return true;
}

QList<gt::SharedFunction> TestDatamodelInterface::sharedFunctions() const
{
    auto lambda = [](double a, double b) {
        return a*b;
    };

    auto sharedFunction =  gt::interface::makeSharedFunction(
        "my_lambda_mult", lambda, "This is a simple multiplication function");

    return {sharedFunction};
}
