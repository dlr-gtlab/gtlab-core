/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef TEST_PROPERTYCONTAINEROBJECT_H
#define TEST_PROPERTYCONTAINEROBJECT_H

#include "gt_object.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"
#include "gt_stringproperty.h"

#include <QObject>

struct TestObject : public GtObject
{
    Q_OBJECT
public:

    TestObject() :
        GtObject()
    {

        GtPropertyStructDefinition envVarStruct("EnvironmentVarsStruct");
        envVarStruct.defineMember("name", gt::makeStringProperty());
        envVarStruct.defineMember("value", gt::makeStringProperty());

        environmentVars.registerAllowedType(envVarStruct);

        registerPropertyStructContainer(environmentVars);
        setObjectName("testobject");
    }

    void addEnvironmentVar(QString name, QString value)
    {
        auto* vars = findPropertyContainer("environmentVars");
        auto& var = vars->newEntry("EnvironmentVarsStruct");

        var.setMemberVal("name", name);
        var.setMemberVal("value", value);
    }

    GtPropertyStructContainer environmentVars{"environmentVars", "variables"};
};


#endif // TEST_PROPERTYCONTAINEROBJECT_H
