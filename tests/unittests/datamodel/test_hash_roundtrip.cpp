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
 *  are used to isolate the cause among:
 *
 *   1. lossy float serialization of list properties (DBL_DIG)
 *   2. non-canonical QVariant serialization (variantToString)
 *   3. property container entry re-ordering in mergePropertyContainer
 *   4. isActive flag lost on memento restore / diff apply
 *   5. hash instability / non-determinism of calcHash itself
 */

#include "gtest/gtest.h"

#include "gt_object.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_boolproperty.h"
#include "gt_doubleproperty.h"
#include "gt_doublelistproperty.h"
#include "gt_variantproperty.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"
#include "test_gt_object.h"
#include "test_propertycontainerobject.h"

#include <QBuffer>
#include <QDataStream>
#include <QPoint>
#include <QPointF>
#include <QUuid>
#include <QRandomGenerator>
#include <QUrl>
#include <QSet>

namespace
{
/**
 * @brief Full-precision, non-round-number doubles
 * such that 15-significant-digit serialization loses information.
 */
QVector<double> makeFullPrecisionDoubles(int n, quint32 seed)
{
    QRandomGenerator gen(seed);

    QVector<double> vals;
    vals.reserve(n);

    for (int i = 0; i < n; ++i)
    {
        double sign = (gen.generate() & 1u) ? 1.0 : -1.0;
        double magnitude = (1.0 + (gen.generate() % 999999) / 9999999.0)
                            * (1e-12 + (gen.generate() % 1000) * 1e-6);
        vals.append(sign * magnitude);
    }

    return vals;
}

double hashOfVariant(const QVariant& var)
{
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    QDataStream ds(&buf);
    ds << var;
    return buf.pos();
}

bool vectorBitwiseEqual(const QVector<double>& a, const QVector<double>& b)
{
    if (a.size() != b.size())
    {
        return false;
    }

    if (a.isEmpty())
    {
        return true;
    }

    return std::memcmp(a.constData(), b.constData(),
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

        objA.setObjectName("TestHashRoundTrip_A");
        objA.setFactory(gtObjectFactory);
    }

    void TearDown() override
    {
        if (objB && objB->parentObject() == &objA)
        {
            objA.removeChild(objB);
        }
        else
        {
            delete objB;
        }
    }

    TestSpecialGtObject objA;
    TestSpecialGtObject* objB{nullptr};
};

/// sanity: no mutation - diff is empty, hashes must agree
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
    ASSERT_TRUE(diff.isNull())
            << "no mutation: diff must be empty";

    EXPECT_EQ(objB->calcHash(), objA->calcHash());
}

/// test 1a: lossy float serialization of QVector<double> through the diff
TEST_F(TestHashRoundTrip, doubleListFullPrecisionHashesMatch)
{
    objA.setDoubleVec(makeFullPrecisionDoubles(20, 0x12345678u));

    objB = objA.clone();
    ASSERT_NE(nullptr, objB);
    objB->setFactory(gtObjectFactory);

    // mutate a DEEP element of the list
    QVector<double> bVals = objB->getDoubleVec();
    bVals[7] = -1.2345678901234567e5;
    bVals[18] = 3.1415926535897932e-3;
    objB->setDoubleVec(bVals);

    QVector<double> aVals = objA.getDoubleVec();
    QBuffer buf;
    buf.open(QIODevice::WriteOnly);
    QDataStream ds(&buf);
    ds << aVals;
    QByteArray aBits = buf.data();
    buf.close();

    GtObjectMemento memA = objA.toMemento(true);
    GtObjectMemento memB = objB->toMemento(true);

    GtObjectMementoDiff diff(memA, memB);
    ASSERT_FALSE(diff.isNull()) << "mutation must produce a diff";
    ASSERT_TRUE(objA.applyDiff(diff)) << "applyDiff failed";

    QVector<double> aAfter = objA.getDoubleVec();
    QVector<double> bValsAfter = objB->getDoubleVec();

    EXPECT_TRUE(vectorBitwiseEqual(aAfter, bValsAfter))
            << "A=" << aAfter.size()
            << " B=" << bValsAfter.size()
            << " firstA=" << (!aAfter.isEmpty() ? aAfter.first() : 0.0)
            << " firstB=" << (!bValsAfter.isEmpty() ? bValsAfter.first()
                                                   : 0.0);

    EXPECT_EQ(objB->calcHash(), objA->calcHash())
            << "hash mismatch after diff round-trip of QVector<double>";
}

/// test 1b: lossy float serialization of GtDoubleListProperty (list path)
TEST_F(TestHashRoundTrip, doubleListPropertyFullPrecisionHashesMatch)
{
    auto* listProp = qobject_cast<GtDoubleListProperty*>(
                objA.findPropertyByName("Double List Property"));
    ASSERT_TRUE(listProp != nullptr);

    listProp->setVal(makeFullPrecisionDoubles(30, 0xA5A5A5A5u));

    objB = objA.clone();
    ASSERT_NE(nullptr, objB);
    objB->setFactory(gtObjectFactory);

    auto* bListProp = qobject_cast<GtDoubleListProperty*>(
                objB->findPropertyByName("Double List Property"));
    ASSERT_TRUE(bListProp != nullptr);

    QVector<double> bVals = bListProp->value<QVector<double>>();
    bVals[11] = 2.7182818284590452;
    bListProp->setVal(bVals);

    GtObjectMemento memA = objA.toMemento(true);
    GtObjectMemento memB = objB->toMemento(true);

    GtObjectMementoDiff diff(memA, memB);
    ASSERT_FALSE(diff.isNull()) << "mutation must produce a diff";
    ASSERT_TRUE(objA.applyDiff(diff)) << "applyDiff failed";

    auto* aListPropAfter = qobject_cast<GtDoubleListProperty*>(
                objA.findPropertyByName("Double List Property"));
    ASSERT_TRUE(aListPropAfter != nullptr);
    QVector<double> aAfter = aListPropAfter->value<QVector<double>>();
    QVector<double> bAfter = bListProp->value<QVector<double>>();

    EXPECT_TRUE(vectorBitwiseEqual(aAfter, bAfter))
            << "values lost precision in diff round-trip: "
            << aAfter.mid(10, 3).join(' ') << " vs "
            << bAfter.mid(10, 3).join(' ');

    EXPECT_EQ(objB->calcHash(), objA->calcHash())
            << "hash mismatch after diff round-trip of GtDoubleListProperty";
}

/// test 1c: QPointF list precision (QList<QPointF> serialization)
TEST_F(TestHashRoundTrip, QPointFListPrecisionSerializationRoundTrip)
{
    QList<QPointF> list = makeFullPrecisionDoubles(10, 0x01020304u)
        .toVector().toStdVector().empty() ? QList<QPointF>()
        : QList<QPointF>();

    QRandomGenerator gen(0x01020304u);
    list.clear();
    for (int i = 0; i < 10; ++i)
    {
        list.append(QPointF((1e-12 + (gen.generate() % 1000) * 1e-6)
                            * (1 + (gen.generate() % 999999) / 9999999.0),
                            (-1e-12 - (gen.generate() % 1000) * 1e-6)
                            * (1 + (gen.generate() % 999999) / 9999999.0)));
    }

    // serialize the same way as the memento XML round-trip:
    // 'g' format with DBL_DIG significant digits (gt_objectio.cpp:
    // listToString<QList<QPointF>>)
    QString serialized;
    for (const QPointF& p : list)
    {
        serialized.append(QString::number(p.x(), 'g', DBL_DIG))
                   .append('_')
                   .append(QString::number(p.y(), 'g', DBL_DIG))
                   .append(';');
    }

    QList<QPointF> restored;
    for (const QString& val : serialized.split(';'))
    {
        const QStringList pvars = val.split('_');
        if (pvars.size() == 2)
        {
            restored.append(QPointF(pvars[0].toDouble(),
                                    pvars[1].toDouble()));
        }
    }

    bool allEqual = restored.size() == list.size();
    if (allEqual)
    {
        for (int i = 0; i < list.size(); ++i)
        {
            if (restored[i].x() != list[i].x()
                    || restored[i].y() != list[i].y())
            {
                allEqual = false;
                break;
            }
        }
    }

    EXPECT_TRUE(allEqual)
            << "QList<QPointF> 'g'/DBL_DIG round-trip is lossy:\n"
            << "  original[" << (!list.isEmpty() ? 0 : -1)
            << "] = (" << list.value(0).x() << ", " << list.value(0).y()
            << ")\n  restored[" << (!restored.isEmpty() ? 0 : -1)
            << "] = (" << restored.value(0).x() << ", "
            << restored.value(0).y() << ")";
}

/// test 2: non-canonical QVariant through GtVariantProperty
TEST_F(TestHashRoundTrip, variantCanonicalformHashesMatch)
{
    objB = objA.clone();
    ASSERT_NE(nullptr, objB);
    objB->setFactory(gtObjectFactory);

    // mutate the variant property on B (QUrl with percent-encoded form,
    // toString() and valueToVariant() may produce different forms)
    objB->setVariant(QUrl(
                "https://user:pass@[::ffff:1a2b:3c4d:5e6f:4455]:8084"
                "/some%20path?query=value&x#fragment"));

    GtObjectMemento memA = objA.toMemento(true);
    GtObjectMemento memB = objB->toMemento(true);

    GtObjectMementoDiff diff(memA, memB);
    ASSERT_FALSE(diff.isNull()) << "mutation must produce a diff";
    ASSERT_TRUE(objA.applyDiff(diff)) << "applyDiff failed";

    QByteArray aBits, bBits;
    {
        QBuffer buf;
        buf.open(QIODevice::WriteOnly);
        QDataStream ds(&buf);
        ds << objA.getVariant();
        aBits = buf.data();
        buf.close();
    }
    {
        QBuffer buf;
        buf.open(QIODevice::WriteOnly);
        QDataStream ds(&buf);
        ds << objB.getVariant();
        bBits = buf.data();
        buf.close();
    }

    EXPECT_EQ(aBits.constData(), bBits.constData())
            << "QVariant changed through variant property round-trip:\n"
            << "  A = " << objA.getVariant().toString()
            << "  B = " << objB.getVariant().toString()
            << "  type A = " << objA.getVariant().type()
            << "  type B = " << objB.getVariant().type();

    EXPECT_EQ(objB->calcHash(), objA->calcHash())
            << "hash mismatch after diff round-trip of QVariant property";
}

/// test 3: property container entry ordering after merge
TEST_F(TestHashRoundTrip, containerOrderingHashesMatch)
{
    auto* parent = new TestObject;
    parent->setFactory(gtObjectFactory);
    ASSERT_TRUE(objA.appendChild(parent));
    parent->addEnvironmentVar("a", "1");
    parent->addEnvironmentVar("b", "2");
    parent->addEnvironmentVar("c", "3");

    QString idA = parent->findPropertyContainer("environmentVars")
                    ->findEntry("a") == parent->findPropertyContainer(
                       "environmentVars")->end()
                    ? QString()
                    : parent->findPropertyContainer("environmentVars")
                        ->findEntry("a")->ident();
    QString idB = parent->findPropertyContainer("environmentVars")
                    ->findEntry("b") == parent->findPropertyContainer(
                       "environmentVars")->end()
                    ? QString()
                    : parent->findPropertyContainer("environmentVars")
                        ->findEntry("b")->ident();
    QString idC = parent->findPropertyContainer("environmentVars")
                    ->findEntry("c") == parent->findPropertyContainer(
                       "environmentVars")->end()
                    ? QString()
                    : parent->findPropertyContainer("environmentVars")
                        ->findEntry("c")->ident();

    objB = objA.clone();
    ASSERT_NE(nullptr, objB);
    objB->setFactory(gtObjectFactory);

    auto* bParent = objB->findDirectChild<TestObject*>();
    ASSERT_TRUE(bParent != nullptr);
    auto* bVars = bParent->findPropertyContainer("environmentVars");
    ASSERT_TRUE(bVars != nullptr);

    // remove middle entry + change first/last entries -> forces
    // re-insertion / deletion inside mergePropertyContainer
    auto iter = bVars->findEntry(idB);
    ASSERT_TRUE(iter != bVars->end()) << "entry 'b' not found in clone";
    bVars->removeEntry(iter);
    bVars->findEntry(idA)->setMemberVal("value", "1.1");
    bVars->findEntry(idC)->setMemberVal("value", "3.3");

    ASSERT_TRUE(bVars->findEntry(idA) != bVars->end());
    ASSERT_TRUE(bVars->findEntry(idC) != bVars->end());
    ASSERT_TRUE(bVars->findEntry(idB) == bVars->end());
    ASSERT_EQ(2, bVars->size());

    GtObjectMemento memA = objA.toMemento(true);
    GtObjectMemento memB = objB->toMemento(true);

    GtObjectMementoDiff diff(memA, memB);
    ASSERT_FALSE(diff.isNull()) << "mutation must produce a diff";
    ASSERT_TRUE(objA.applyDiff(diff)) << "applyDiff failed";

    auto* aVars = parent->findPropertyContainer("environmentVars");
    ASSERT_TRUE(aVars != nullptr);

    QList<QPair<QString, QString>> entriesA, entriesB;
    for (const auto& e : *aVars)
    {
        entriesA.append(qMakePair(e.memberValToVariant("name").toString(),
                                  e.memberValToVariant("value").toString()));
    }
    for (const auto& e : *bVars)
    {
        entriesB.append(qMakePair(e.memberValToVariant("name").toString(),
                                  e.memberValToVariant("value").toString()));
    }

    QSet<QString> expected;
    for (const auto& e : entriesB)
    {
        expected.insert(e.first + "=" + e.second);
    }
    for (const auto& e : entriesA)
    {
        EXPECT_TRUE(expected.contains(e.first + "=" + e.second))
                << "merged container lost or altered entry: "
                << e.first << "=" << e.second;
    }

    EXPECT_EQ(objB->calcHash(), objA->calcHash())
            << "hash mismatch after diff round-trip of property container";
}

/// test 4: isActive flag must survive the memento/diff round-trip
TEST_F(TestHashRoundTrip, isActiveSurvivesHashesMatch)
{
    auto* boolProp = qobject_cast<GtBoolProperty*>(
                objA.findPropertyByName("Test Bool"));
    ASSERT_TRUE(boolProp != nullptr);
    objA.setBool(true);

    objB = objA.clone();
    ASSERT_NE(nullptr, objB);
    objB->setFactory(gtObjectFactory);

    auto* bBoolProp = qobject_cast<GtBoolProperty*>(
                objB->findPropertyByName("Test Bool"));
    ASSERT_TRUE(bBoolProp != nullptr);

    // mutate value on B AND drop active state on B
    boolProp->setActive(false);
    bBoolProp->setActive(false);
    objB->setDouble(77.7);

    GtObjectMemento memA = objA.toMemento(true);
    GtObjectMemento memB = objB->toMemento(true);

    GtObjectMementoDiff diff(memA, memB);
    ASSERT_FALSE(diff.isNull()) << "mutation must produce a diff";
    ASSERT_TRUE(objA.applyDiff(diff)) << "applyDiff failed";
    QObject::disconnect();

    boolProp = qobject_cast<GtBoolProperty*>(
                objA.findPropertyByName("Test Bool"));
    EXPECT_TRUE(boolProp->isActive() == false)
            << "isActive flag not restored after applyDiff (memento reader "
            << "TODO: gt_objectmemento.cpp 'should this be set by the "
            << "reader ???')";
    EXPECT_DOUBLE_EQ(77.7, objA.getDouble());

    EXPECT_EQ(objB->calcHash(), objA->calcHash())
            << "hash mismatch caused by lost isActive flag";
}

/// test 5: calcHash must be deterministic and stable across repeated calls
TEST_F(TestHashRoundTrip, calcHashIsDeterministic)
{
    objA.setDouble(1.0 / 3.0);
    objA.setBool(false);
    objA.setDoubleVec(makeFullPrecisionDoubles(5, 0xCAFEBABEu));

    GtObjectMemento mem = objA.toMemento(true);
    mem.calculateHashes();
    QByteArray mementoHashBytes = mem.fullHash();

    QByteArray hash1, hash2, hash3;
    for (int i = 0; i < 3; ++i)
    {
        GtObjectMemento m = objA.toMemento(true);
        m.calculateHashes();
        QByteArray h = m.fullHash();

        if (i == 0) { hash1 = h; }
        if (i == 1) { hash2 = h; }
        if (i == 2) { hash3 = h; }
    }

    EXPECT_EQ(hash1.constData(), hash2.constData())
            << "calcHash non-deterministic between two consecutive calls";
    EXPECT_EQ(hash2.constData(), hash3.constData())
            << "calcHash non-deterministic between call two and three";
    EXPECT_EQ(mem.fullHash().toHex(), objA.calcHash())
            << "GtObject::calcHash differs from toMemento()+calculateHashes()";
}
