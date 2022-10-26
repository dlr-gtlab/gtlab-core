#ifndef TEST_PROPERTYCONTAINEROBJECT_H
#define TEST_PROPERTYCONTAINEROBJECT_H

#include "gt_object.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"
#include "gt_stringproperty.h"

#include <QObject>

struct TestObject : public GtObject
{
    TestObject() :
        GtObject()
    {

        GtPropertyStructDefinition envVarStruct("EnvironmentVarsStruct");
        envVarStruct.defineMember("name", makeStringProperty());
        envVarStruct.defineMember("value", makeStringProperty());

        environmentVars.registerAllowedType(envVarStruct);

        registerPropertyStructContainer(environmentVars);
        setObjectName("testobject");
    }

    void addEnvironmentVar(QString name, QString value)
    {
        auto* vars = findDynamicSizeProperty("environmentVars");
        auto& var = vars->newEntry("EnvironmentVarsStruct");

        var.setMemberVal("name", name);
        var.setMemberVal("value", value);
    }

    GtPropertyStructContainer environmentVars{"environmentVars"};
};


#endif // TEST_PROPERTYCONTAINEROBJECT_H
