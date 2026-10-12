/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 12.10.2026
 */

#include "gtest/gtest.h"

#include <QSet>
#include <QUuid>
#include <memory>

#include "gt_accesstracking.h"
#include "gt_colorproperty.h"
#include "gt_doublelistproperty.h"
#include "gt_doubleproperty.h"
#include "gt_object.h"

namespace
{

/// Runs the given value read with a fresh tracker active and returns the
/// tracker's set of accessed objects.
template<class ReadFn>
QSet<QUuid>
accessedDuring(ReadFn read)
{
    auto tracker = std::make_unique<GtAccessTracker>();

    {
        GtAccessTrackerScope scope(tracker.get());
        read();
    }

    return tracker->accessedObjects();
}

}

TEST(GtPropertyAccessTracking, mutableReferenceGetTracksOwner)
{
    GtObject owner;
    GtDoubleProperty prop{"value", "Value"};
    prop.setOwnerObject(&owner);

    const QSet<QUuid> expected{QUuid(owner.uuid())};

    EXPECT_EQ(accessedDuring([&]() { (void)prop.get(); }), expected);
}

TEST(GtPropertyAccessTracking, constReferenceGetTracksOwner)
{
    GtObject owner;
    GtDoubleProperty prop{"value", "Value"};
    prop.setOwnerObject(&owner);
    const GtDoubleProperty& constProp = prop;

    const QSet<QUuid> expected{QUuid(owner.uuid())};

    EXPECT_EQ(accessedDuring([&]() { (void)constProp.get(); }), expected);
}

TEST(GtPropertyAccessTracking, operatorCallTracksOwner)
{
    GtObject owner;
    GtDoubleProperty prop{"value", "Value"};
    prop.setOwnerObject(&owner);

    const QSet<QUuid> expected{QUuid(owner.uuid())};

    EXPECT_EQ(accessedDuring([&]() { (void)prop(); }), expected);
}

TEST(GtPropertyAccessTracking, conversionOperatorTracksOwner)
{
    GtObject owner;
    GtDoubleProperty prop{"value", "Value"};
    prop.setOwnerObject(&owner);

    const QSet<QUuid> expected{QUuid(owner.uuid())};

    // the conversion operator delegates to getVal()
    EXPECT_EQ(accessedDuring([&]() { const double value = prop; (void)value; }), expected);
}

TEST(GtPropertyAccessTracking, getValTracksOwner)
{
    GtObject owner;
    GtDoubleProperty prop{"value", "Value"};
    prop.setOwnerObject(&owner);

    const QSet<QUuid> expected{QUuid(owner.uuid())};

    EXPECT_EQ(accessedDuring([&]() { (void)prop.getVal(); }), expected);
}

TEST(GtPropertyAccessTracking, getValWithUnitTracksOwner)
{
    GtObject owner;
    GtDoubleProperty prop{"value", "Value"};
    prop.setOwnerObject(&owner);

    const QSet<QUuid> expected{QUuid(owner.uuid())};

    // only the access is asserted here, not the unit conversion outcome
    EXPECT_EQ(accessedDuring([&]() {
        bool ok = true;
        (void)prop.getVal(QStringLiteral("km/h"), &ok);
    }), expected);
}

TEST(GtPropertyAccessTracking, valueToVariantWithUnitTracksOwner)
{
    GtObject owner;
    GtDoubleProperty prop{"value", "Value"};
    prop.setOwnerObject(&owner);

    const QSet<QUuid> expected{QUuid(owner.uuid())};

    // only the access is asserted here, not the unit conversion outcome
    EXPECT_EQ(accessedDuring([&]() {
        bool ok = true;
        (void)prop.valueToVariant(QStringLiteral("km/h"), &ok);
    }), expected);
}

TEST(GtPropertyAccessTracking, noArgValueToVariantTracksOwner)
{
    GtObject owner;
    GtDoubleProperty prop{"value", "Value"};
    prop.setOwnerObject(&owner);

    const QSet<QUuid> expected{QUuid(owner.uuid())};

    // qualified call: the derived class' valueToVariant(QString, bool*)
    // hides the base class' no-arg overload
    EXPECT_EQ(accessedDuring([&]() { (void)prop.GtAbstractProperty::valueToVariant(); }), expected);
}

TEST(GtPropertyAccessTracking, colorPropertyValueToVariantTracksOwner)
{
    GtObject owner;
    GtColorProperty prop{"color", "Color", "A color", Qt::red};
    prop.setOwnerObject(&owner);

    const QSet<QUuid> expected{QUuid(owner.uuid())};

    // the color override reads m_value directly instead of delegating to
    // getVal(), so it needs its own instrumentation
    EXPECT_EQ(accessedDuring([&]() {
        bool ok = true;
        (void)prop.valueToVariant(QString(), &ok);
    }), expected);
}

TEST(GtPropertyAccessTracking, doubleListPropertyValueToVariantTracksOwner)
{
    GtObject owner;
    GtDoubleListProperty prop{"list", "List"};
    prop.setOwnerObject(&owner);

    const QSet<QUuid> expected{QUuid(owner.uuid())};

    // the double list override reads m_value directly instead of delegating
    // to getVal(), so it needs its own instrumentation
    EXPECT_EQ(accessedDuring([&]() {
        bool ok = true;
        (void)prop.valueToVariant(QString(), &ok);
    }), expected);
}

TEST(GtPropertyAccessTracking, propertyWithoutOwnerTracksNothing)
{
    GtDoubleProperty prop{"value", "Value"};

    // no owner: the tracking is a no-op and the read is unchanged
    EXPECT_TRUE(accessedDuring([&]() { (void)prop.getVal(); }).isEmpty());
    EXPECT_DOUBLE_EQ(prop.getVal(), prop.get());
}

TEST(GtPropertyAccessTracking, noActiveTrackerLeavesReadsUntouched)
{
    GtObject owner;
    GtDoubleProperty prop{"value", "Value"};
    prop.setOwnerObject(&owner);

    const double reference = prop.getVal();

    // without an active tracker every read must behave exactly as before
    EXPECT_DOUBLE_EQ(prop.getVal(), reference);
    EXPECT_DOUBLE_EQ(prop.get(), reference);
    EXPECT_DOUBLE_EQ(prop(), reference);
    EXPECT_TRUE(prop.GtAbstractProperty::valueToVariant().isValid());
}
