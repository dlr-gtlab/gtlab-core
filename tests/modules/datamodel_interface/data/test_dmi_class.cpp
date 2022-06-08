/* GTlab - Gas Turbine laboratory
 * Source File: test_dmi_class.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 15.11.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#include "test_dmi_class.h"

#include "gt_functional_interface.h"
#include "gt_dynamicinterfacehandler.h"

TestDmClass::TestDmClass()
{
    auto lambda = [](double a, double b) {
        return a*b;
    };

    gtlab::interface::priv::register_function(
                "my_lambda_mult", lambda,
                "This is a simple multiplication function");

    setObjectName("DMI Object");
}
