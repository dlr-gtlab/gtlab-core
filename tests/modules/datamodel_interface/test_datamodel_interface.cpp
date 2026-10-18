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
#include "gt_doubleproperty.h"

/**
 * generated (v 0.1)
 */

#include "test_datamodel_interface.h"

namespace
{
class TestDynamicDoubleProperty : public GtDoubleProperty
{
public:
    using GtDoubleProperty::GtDoubleProperty;
};
} // namespace

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

void
TestDatamodelInterface::registerDynamicPropertyTypes()
{
    m_dynamicPropertyRegistrations.emplace_back(
        GtDynamicPropertyFactory::registerType(
            QStringLiteral("test.datamodel.double"),
            [](const GtAbstractProperty& property) {
                return dynamic_cast<const TestDynamicDoubleProperty*>(&property)
                       != nullptr;
            },
            [](const GtAbstractProperty&) { return QVariantMap{}; },
            [](const QString& ident, const QString& name,
               const QVariantMap& definition)
                -> std::unique_ptr<GtAbstractProperty> {
                if (!definition.isEmpty()) return {};
                return std::make_unique<TestDynamicDoubleProperty>(ident,
                                                                    name);
            }));
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
