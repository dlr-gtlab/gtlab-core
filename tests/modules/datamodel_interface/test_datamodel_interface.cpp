/* GTlab - Gas Turbine laboratory
 * Source File: test_datamodel_interface
 * copyright 2009-2019 by DLR
 *
 * Created on: 27.05.2020
 * Author: S. Reitenbach
 */

#include "test_dmi_package.h"
#include "test_dmi_class.h"

#include "gt_functional_interface.h"
#include "gt_dynamicinterface.h"

/**
 * generated (v 0.1)
 */

#include "test_datamodel_interface.h"

GtVersionNumber
TestDatamodelInterface::version()
{
    return GtVersionNumber(1);
}

QString
TestDatamodelInterface::ident() const
{
    return "Test Datamodel Interface";
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

    metaData << GT_METADATA(TestDmClass);

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

QList<gtlab::InterfaceFunction> TestDatamodelInterface::sharedFunctions() const
{
    auto lambda = [](double a, double b) {
        return a*b;
    };

    auto sharedFunction =  gtlab::interface::make_interface_function(
        "my_lambda_mult", lambda, "This is a simple multiplication function");

    return {sharedFunction};
}
