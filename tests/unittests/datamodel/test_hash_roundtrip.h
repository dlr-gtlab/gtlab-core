/* GTlab - Gas Turbine laboratory
 *
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 */

#ifndef TEST_HASH_ROUNDTRIP_H
#define TEST_HASH_ROUNDTRIP_H

#include <QObject>
#include <QVariant>
#include <QVector>

#include "gt_object.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"
#include "gt_stringproperty.h"

class TestHashContainerObject : public GtObject
{
    Q_OBJECT
public:
    Q_INVOKABLE TestHashContainerObject()
    {
        GtPropertyStructDefinition s("EnvironmentVarsStruct");
        s.defineMember("name", gt::makeStringProperty());
        s.defineMember("value", gt::makeStringProperty());
        envVars.registerAllowedType(s);
        registerPropertyStructContainer(envVars);
        setObjectName("testhashcontainerobject");
    }

    void addEnvVar(QString name, QString value)
    {
        auto& var = envVars.newEntry("EnvironmentVarsStruct");
        var.setMemberVal("name", name);
        var.setMemberVal("value", value);
    }

    GtPropertyStructContainer envVars{"envVars", "environment vars"};
};

/// object exposing a raw QVariant as a Qt meta property,
/// so the diff path must serialize it through variantToString
class TestHashVariantObject : public GtObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant myVariant MEMBER m_variant)
public:
    Q_INVOKABLE TestHashVariantObject()
    {
        setObjectName("testhashvariantobject");
    }

    QVariant m_variant;
};

/// object exposing a full-precision QPointF and QList<QPointF>
/// as Qt meta properties
class TestHashPointObject : public GtObject
{
    Q_OBJECT
    Q_PROPERTY(QPointF myPoint MEMBER m_point)
    Q_PROPERTY(QList<QPointF> points MEMBER m_points)
public:
    Q_INVOKABLE TestHashPointObject()
    {
        setObjectName("testhashpointobject");
    }

    QPointF m_point;
    QList<QPointF> m_points;
};

/// object exposing a QList<double> as a Qt meta property
/// no gtproperty uses QList<double> so not an issue in practice
/// (on Qt 5 QList<double> and QVector<double> are distinct types)
class TestHashDoubleListObject : public GtObject
{
    Q_OBJECT
    Q_PROPERTY(QList<double> values MEMBER m_values)
public:
    Q_INVOKABLE TestHashDoubleListObject()
    {
        setObjectName("testhashdoublelistobject");
    }

    QList<double> m_values;
};

#endif // TEST_HASH_ROUNDTRIP_H
