/* GTlab - Gas Turbine laboratory
 *
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * SPDX-License-Identifier: MPL-2.0+
 */

/**
 * @brief Provenance hash invariant tests.
 *
 * Reproduces the production pipeline (GtCoreProcessExecutor::
 * handleTaskFinishedHelper) where a calculator mutates a sandbox
 * clone of a linked object and the diff is merged back into the
 * live data model:
 *
 *   A = live object, B = A.clone(), B mutated,
 *   diff = GtObjectMementoDiff(A.toMemento(true), B.toMemento(true)),
 *   A.applyDiff(diff)
 *
 * After the merge A must be semantically identical to B, i.e.
 * A->calcHash() == B->calcHash(). In production this invariant
 * broke - these tests isolate the suspected causes:
 *
 *   1. lossy double serialization of lists ('g' format, DBL_DIG)
 *   2. non-canonical scalar QVariant serialization (var.toString())
 *   3. property container entry add/remove handling in the diff
 *   4. added/removed properties silently dropped by the diff
 *   5. non-determinism of GtObject::calcHash itself
 *
 * Each test either proves the suspect is NOT the cause (hashes
 * match) or reproduces the production failure (hash mismatch with
 * a visible value difference in the failure message).
 */

#include "gtest/gtest.h"

#include "gt_object.h"
#include "gt_boolproperty.h"
#include "gt_doublelistproperty.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"

#include "test_gt_object.h"
#include "test_hash_roundtrip.h"

#include <cstring>
#include <QRandomGenerator>
#include <QUuid>

namespace
{

/// doubles whose exact representation generally needs 16 significant
/// digits, i.e. not round-trippable with 15 (DBL_DIG) digits
QVector<double> makeFullPrecisionDoubles(int n, quint32 seed)
{
    QRandomGenerator gen(seed);
    QVector<double> vals;
    vals.reserve(n);

    for (int i = 0; i < n; ++i)
    {
        double magnitude = 1.0 + (gen.generate() % 9999999) / 9999999.0;
        double scale = (1.0 + (gen.generate() % 1001) * 1e-16) * 1e6;
        double sign = (gen.generate() & 0x1u) ? 1.0 : -1.0;
        vals.append(sign * magnitude * scale);
    }
    return vals;
}

bool vectorsBitwiseEqual(const QVector<double>& a,
                         const QVector<double>& b)
{
    if (a.size() != b.size())
    {
        return false;
    }
    return a.isEmpty() || std::memcmp(a.constData(),
                                      b.constData(),
                                      sizeof(double) * a.size()) == 0;
}

} // namespace

class TestHashRoundTrip : public ::testing::Test
{
protected:
    void SetUp() override
    {
        if (!gtObjectFactory->knownClass("TestSpecialGtObject"))
        {
            gtObjectFactory->registerClass(
                        TestSpecialGtObject::staticMetaObject);
        }
        auto registerClassOnce = [this](QMetaObject meta,
                                        const char* name) {
            if (!gtObjectFactory->knownClass(QString(name)))
            {
                gtObjectFactory->registerClass(meta);
            }
        };
        registerClassOnce(TestHashContainerObject::staticMetaObject,
                          "TestHashContainerObject");
        registerClassOnce(TestHashVariantObject::staticMetaObject,
                          "TestHashVariantObject");
        registerClassOnce(TestHashPointObject::staticMetaObject,
                          "TestHashPointObject");
    }

    void TearDown() override
    {
        delete objB;
        delete oB;
    }

    TestSpecialGtObject* b()
    {
        return qobject_cast<TestSpecialGtObject*>(objB);
    }

    TestSpecialGtObject objA;
    GtObject* objB{nullptr};
    TestHashPointObject* oB{nullptr};
};

/// no mutation: diff must be empty, hashes must agree
TEST_F(TestHashRoundTrip, noOpDiffHashesMatch)
{
    objA.setDouble(10.5);
    objA.setBool(true);

    objB = objA.clone();
    ASSERT_NE(nullptr, objB);
    objB->setFactory(gtObjectFactory);

    GtObjectMemento memA = objA.toMemento(true);
    GtObjectMemento memB = objB->toMemento(true);

    GtObjectMementoDiff diff(memA, memB);
    EXPECT_TRUE(diff.isNull()) << "no mutation: diff must be empty";

    EXPECT_EQ(objB->calcHash(), objA.calcHash());
}

/// suspect 1a: full-precision QVector<double> through the diff
TEST_F(TestHashRoundTrip, doubleVecFullPrecisionHashesMatch)
{
    objA.setDoubleVec(makeFullPrecisionDoubles(20, 0x12345678u));

    objB = objA.clone();
    ASSERT_NE(nullptr, objB);
    objB->setFactory(gtObjectFactory);

    QVector<double> bVals = b()->getDoubleVec();
    bVals[7] = -1.2345678901234567e5;
    bVals[18] = 3.1415926535897932e15;
    b()->setDoubleVec(bVals);

    GtObjectMemento memA = objA.toMemento(true);
    GtObjectMemento memB = objB->toMemento(true);

    GtObjectMementoDiff diff(memA, memB);
    ASSERT_FALSE(diff.isNull()) << "mutation must produce a diff";
    ASSERT_TRUE(objA.applyDiff(diff)) << "applyDiff failed";
    //objA.setDoubleVec(bVals);

    QVector<double> aAfter = objA.getDoubleVec();
    QVector<double> bAfter = b()->getDoubleVec();

    EXPECT_TRUE(vectorsBitwiseEqual(aAfter, bAfter))
            << "QVector<double> lost precision in diff round-trip:\n"
            << "  [7]:  A=" << aAfter.value(7)
            << " B=" << bAfter.value(7)
            << "\n [18]: A=" << aAfter.value(18)
            << " B=" << bAfter.value(18);

    EXPECT_EQ(objB->calcHash(), objA.calcHash())
            << "suspect 1 confirmed: double list serialization lossy";
}

/// suspect 1b: GtDoubleListProperty (GTlab list property path)
TEST_F(TestHashRoundTrip, doubleListPropertyFullPrecisionHashesMatch)
{
    auto* prop = qobject_cast<GtDoubleListProperty*>(
                objA.findPropertyByName("Double List Property"));
    ASSERT_TRUE(prop != nullptr);
    prop->setVal(makeFullPrecisionDoubles(30, 0xA5A5A5A5u));

    objB = objA.clone();
    ASSERT_NE(nullptr, objB);
    objB->setFactory(gtObjectFactory);

    auto* bProp = qobject_cast<GtDoubleListProperty*>(
                objB->findPropertyByName("Double List Property"));
    ASSERT_TRUE(bProp != nullptr);

    QVector<double> bVals = bProp->get();
    bVals[11] = 2.7182818284590452;
    bProp->setVal(bVals);

    GtObjectMemento memA = objA.toMemento(true);
    GtObjectMemento memB = objB->toMemento(true);

    GtObjectMementoDiff diff(memA, memB);
    ASSERT_FALSE(diff.isNull()) << "mutation must produce a diff";
    ASSERT_TRUE(objA.applyDiff(diff)) << "applyDiff failed";

    auto* aPropAfter = qobject_cast<GtDoubleListProperty*>(
                objA.findPropertyByName("Double List Property"));
    ASSERT_TRUE(aPropAfter != nullptr);
    QVector<double> aAfter = aPropAfter->get();
    QVector<double> bAfter = bProp->get();

    EXPECT_TRUE(vectorsBitwiseEqual(aAfter, bAfter))
            << "GtDoubleListProperty lost precision in diff round-trip:\n"
            << "  [11]: A=" << aAfter.value(11)
            << " B=" << bAfter.value(11);

    EXPECT_EQ(objB->calcHash(), objA.calcHash())
            << "suspect 1 confirmed: GtDoubleListProperty "
            << "serialization lossy";
}

/// suspect 2a: scalar double through GtDoubleProperty
/// (non-list path: variantToString -> QString -> propertyToVariant)
TEST_F(TestHashRoundTrip, fullPrecisionDoubleScalarHashesMatch)
{
    objA.setDouble(10.0);

    objB = objA.clone();
    ASSERT_NE(nullptr, objB);
    objB->setFactory(gtObjectFactory);

    const double target = 1.0 + 1e-16 * 3141.5;
    b()->setDouble(target);

    GtObjectMementoDiff diff(objA.toMemento(true),
                             objB->toMemento(true));
    ASSERT_FALSE(diff.isNull()) << "double change must produce a diff";
    ASSERT_TRUE(objA.applyDiff(diff)) << "applyDiff failed";

    const double aVal = objA.getDouble();
    const double bVal = b()->getDouble();

    EXPECT_TRUE(std::memcmp(&aVal, &bVal, sizeof(double)) == 0)
            << "double property lost precision through the diff:\n"
            << "  A=" << aVal << "\n  B=" << bVal;

    EXPECT_EQ(objB->calcHash(), objA.calcHash())
            << "suspect 2 confirmed: scalar double serialization lossy";
}

/// suspect 2b: QPointF variant through Qt meta property
/// (variantToString: "x_y" via QString::number default precision)
TEST_F(TestHashRoundTrip, QPointFVariantPrecisionHashesMatch)
{
    TestHashPointObject oA;
    oA.setFactory(gtObjectFactory);
    oA.m_point = QPointF(1.2345678901234567e-15, 9.8765432109876543e12);

    oB = qobject_cast<TestHashPointObject*>(oA.clone());
    ASSERT_NE(nullptr, oB);
    oB->setFactory(gtObjectFactory);

    const QPointF bPoint(-3.1415926535897932e-16, 2.718281828459045e15);
    oB->m_point = bPoint;

    GtObjectMementoDiff diff(oA.toMemento(true), oB->toMemento(true));
    ASSERT_FALSE(diff.isNull()) << "QPointF change must produce a diff";
    ASSERT_TRUE(oA.applyDiff(diff)) << "applyDiff failed";

    EXPECT_TRUE(oA.m_point == bPoint)
            << "QPointF lost precision through the diff:\n"
            << "  A = (" << oA.m_point.x() << ", " << oA.m_point.y()
            << ")\n  B = (" << bPoint.x() << ", " << bPoint.y() << ")";

    EXPECT_EQ(oB->calcHash(), oA.calcHash())
            << "suspect 2 confirmed: QVariant (QPointF) "
            << "serialization is lossy";
}

/// suspect 3: container entry add/remove/change through the diff.
/// Known code issue: GtObjectMementoDiff::detectPropertyChanges
/// ignores added/removed properties ("do nothing for now") ->
/// A keeps removed entries, does not gain added ones -> hash break.
TEST_F(TestHashRoundTrip, containerEntryAddRemoveHashesMatch)
{
    auto* a = new TestHashContainerObject;
    a->setFactory(gtObjectFactory);
    a->addEnvVar("a", "1");
    a->addEnvVar("b", "2");
    a->addEnvVar("c", "3");

    auto idsOf = [](const GtPropertyStructContainer& c) {
        QStringList list;
        for (auto it = c.begin(); it != c.end(); ++it)
        {
            list << it->ident();
        }
        return list;
    };
    const QStringList idsA = idsOf(a->envVars);
    ASSERT_EQ(3, int(idsA.size()));

    objB = a->clone();
    ASSERT_NE(nullptr, objB);
    objB->setFactory(gtObjectFactory);

    auto* b = qobject_cast<TestHashContainerObject*>(objB);
    ASSERT_TRUE(b != nullptr);
    ASSERT_EQ(3, int(b->envVars.size()));

    const QStringList bIds = idsOf(b->envVars);

    // remove middle entry on B, change first & last entries
    auto iter = b->envVars.findEntry(bIds[1]);
    ASSERT_TRUE(iter != b->envVars.end()) << "cloned entry not found by id";
    b->envVars.removeEntry(iter);
    b->envVars.findEntry(bIds[0])->setMemberVal("value", "1.1");
    b->envVars.findEntry(bIds[2])->setMemberVal("value", "3.3");
    ASSERT_EQ(2, int(b->envVars.size()));

    GtObjectMementoDiff diff(a->toMemento(true),
                             objB->toMemento(true));
    ASSERT_FALSE(diff.isNull()) << "mutation must produce a diff";
    ASSERT_TRUE(a->applyDiff(diff)) << "applyDiff failed";

    auto dump = [](const GtPropertyStructContainer& c) {
        QStringList out;
        for (auto it = c.begin(); it != c.end(); ++it)
        {
            out << it->ident() << "="
                << it->getMemberValToVariant("value").toString();
        }
        return out;
    };

    const QStringList entriesA = dump(a->envVars);
    const QStringList entriesB = dump(b->envVars);

    EXPECT_EQ(entriesB, entriesA)
            << "container entries diverged after diff apply:\n"
            << "  A=" << entriesA.join(", ").toStdString()
            << "\n  B=" << entriesB.join(", ").toStdString()
            << "\n  (diff drops added/removed entries - "
            "detectPropertyChanges: 'do nothing for now')";

    EXPECT_EQ(objB->calcHash(), a->calcHash())
            << "suspect 3 confirmed: container entry add/remove "
            "breaks hash";
}

/// suspect 4 sanity: an existing scalar property change through the
/// diff must be applied exactly (rules out the plain property path)
TEST_F(TestHashRoundTrip, simplePropertyChangeHashesMatch)
{
    objA.setDouble(1.0);
    objA.setString("abc");

    objB = objA.clone();
    ASSERT_NE(nullptr, objB);
    objB->setFactory(gtObjectFactory);

    b()->setDouble(2.5);
    b()->setString("def");

    GtObjectMementoDiff diff(objA.toMemento(true),
                             objB->toMemento(true));
    ASSERT_FALSE(diff.isNull()) << "mutation must produce a diff";
    ASSERT_TRUE(objA.applyDiff(diff)) << "applyDiff failed";

    EXPECT_DOUBLE_EQ(b()->getDouble(), objA.getDouble());
    ASSERT_EQ(b()->getString(), objA.getString());

    EXPECT_EQ(objB->calcHash(), objA.calcHash())
            << "simple property change round-trip must be lossless";
}

/// test 5: calcHash must be deterministic across repeated calls
TEST_F(TestHashRoundTrip, calcHashIsDeterministic)
{
    objA.setDouble(1.0 / 3.0);
    objA.setBool(false);
    objA.setDoubleVec(makeFullPrecisionDoubles(5, 0xCAFEBABEu));

    GtObjectMemento m1 = objA.toMemento(true);
    m1.calculateHashes();
    GtObjectMemento m2 = objA.toMemento(true);
    m2.calculateHashes();

    EXPECT_EQ(m1.fullHash().toHex(), m2.fullHash().toHex())
            << "toMemento + calculateHashes non-deterministic";
    EXPECT_EQ(m1.fullHash().toHex(), objA.calcHash())
            << "GtObject::calcHash differs from "
            << "toMemento()+calculateHashes()";
    EXPECT_EQ(objA.calcHash(), objA.calcHash())
            << "calcHash non-deterministic";
}
