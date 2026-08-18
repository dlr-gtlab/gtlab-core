/* GTlab - Gas Turbine laboratory
 *
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 */

/**
 * @brief These tests verify the provenance hash invariant:
 *
 *  A = object, B = A.clone() (deep copy), B is mutated,
 *  diff(A, B) is computed and applied back onto A.
 *  Afterwards, A and B are semantically identical - therefore
 *  A->calcHash() must equal B->calcHash().
 *
 *  This mirrors the production pipeline
 *  (GtCoreProcessExecutor::handleTaskFinishedHelper) where a calculator
 *  mutates a restored clone and the diff is merged back into the
 *  live data model. In production the hashes diverged - these tests
 *  isolate the cause among:
 *
 *   1. lossy float serialization of list properties (DBL_DIG)
 *   2. non-canonical QVariant serialization (variantToString)
 *   3. property container entry re-ordering in mergePropertyContainer
 *   4. isActive flag lost on memento restore / diff apply
 *   5. non-determinism of calcHash itself
 */

#ifndef TEST_HASH_ROUNDTRIP_H
#define TEST_HASH_ROUNDTRIP_H

#include "gt_object.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"
#include "gt_stringproperty.h"

class TestContainerObject : public GtObject
{
    Q_OBJECT
public:
    TestContainerObject() :
        GtObject()
    {
        GtPropertyStructDefinition envVarStruct("EnvironmentVarsStruct");
        envVarStruct.defineMember("name", gt::makeStringProperty());
        envVarStruct.defineMember("value", gt::makeStringProperty());

        environmentVars.registerAllowedType(envVarStruct);

        registerPropertyStructContainer(environmentVars);
        setObjectName("testcontainerobject");
    }

    void addEnvironmentVar(QString name, QString value)
    {
        auto& var = environmentVars.newEntry("EnvironmentVarsStruct");
        var.setMemberVal("name", name);
        var.setMemberVal("value", value);
    }

    GtPropertyStructContainer environmentVars{"environmentVars", "variables"};
};

#endif // TEST_HASH_ROUNDTRIP_H
