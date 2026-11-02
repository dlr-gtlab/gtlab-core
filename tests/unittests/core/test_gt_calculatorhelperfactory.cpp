/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_calculatorhelperfactory.h"

#include "gt_object.h"

namespace
{

    class CalcHelperParent : public GtObject
    {
        Q_OBJECT

    public:
        Q_INVOKABLE CalcHelperParent() = default;
    };

    class CalcHelperParentOther : public GtObject
    {
        Q_OBJECT

    public:
        Q_INVOKABLE CalcHelperParentOther() = default;
    };

    class CalcHelper : public GtObject
    {
        Q_OBJECT

    public:
        Q_INVOKABLE CalcHelper() = default;
    };

    class CalcHelperOther : public GtObject
    {
        Q_OBJECT

    public:
        Q_INVOKABLE CalcHelperOther() = default;
    };

} // namespace

TEST(GtCalculatorHelperFactory, ConnectsHelpersToParentClass)
{
    ASSERT_TRUE(gtCalculatorHelperFactory->connectHelper(
        GT_CLASSNAME(CalcHelperParent), GT_METADATA(CalcHelper)));

    // duplicate connections are accepted without adding the helper twice
    ASSERT_TRUE(gtCalculatorHelperFactory->connectHelper(
        GT_CLASSNAME(CalcHelperParent), GT_METADATA(CalcHelper)));

    EXPECT_EQ(gtCalculatorHelperFactory->connectedHelper(
                  GT_CLASSNAME(CalcHelperParent)),
              QStringList{GT_CLASSNAME(CalcHelper)});

    EXPECT_FALSE(gtCalculatorHelperFactory->connectHelper(
        QString(), GT_METADATA(CalcHelper)));
}

TEST(GtCalculatorHelperFactory, CreatesConnectedHelper)
{
    ASSERT_TRUE(gtCalculatorHelperFactory->connectHelper(
        GT_CLASSNAME(CalcHelperParent), GT_METADATA(CalcHelper)));

    CalcHelperParent parent;
    GtObject* helper = gtCalculatorHelperFactory->makeCalculatorHelper(
        GT_CLASSNAME(CalcHelper), QStringLiteral("helper"), parent);

    ASSERT_NE(helper, nullptr);
    EXPECT_STREQ(helper->metaObject()->className(), GT_CLASSNAME(CalcHelper));
    EXPECT_EQ(helper->objectName(), QStringLiteral("helper"));
    EXPECT_EQ(helper->parentObject(), &parent);
}

TEST(GtCalculatorHelperFactory, ReusesExistingConnectedHelper)
{
    ASSERT_TRUE(gtCalculatorHelperFactory->connectHelper(
        GT_CLASSNAME(CalcHelperParent), GT_METADATA(CalcHelper)));

    CalcHelperParent parent;
    GtObject* first = gtCalculatorHelperFactory->makeCalculatorHelper(
        GT_CLASSNAME(CalcHelper), QStringLiteral("helper"), parent);
    ASSERT_NE(first, nullptr);

    GtObject* second = gtCalculatorHelperFactory->makeCalculatorHelper(
        GT_CLASSNAME(CalcHelper), QStringLiteral("helper"), parent);

    // idempotent: no duplicate helper is created
    EXPECT_EQ(first, second);
    EXPECT_EQ(parent.findDirectChildren<CalcHelper*>().size(), 1);
}

TEST(GtCalculatorHelperFactory, RejectsInvalidRequests)
{
    ASSERT_TRUE(gtCalculatorHelperFactory->connectHelper(
        GT_CLASSNAME(CalcHelperParent), GT_METADATA(CalcHelper)));

    CalcHelperParent parent;

    // empty class name
    EXPECT_EQ(gtCalculatorHelperFactory->makeCalculatorHelper(
                  QString(), QStringLiteral("helper"), parent),
              nullptr);

    // helper class not connected to the parent class
    EXPECT_EQ(gtCalculatorHelperFactory->makeCalculatorHelper(
                  GT_CLASSNAME(CalcHelperOther),
                  QStringLiteral("otherHelper"), parent),
              nullptr);

    // helper is not connected to this parent class
    CalcHelperParentOther otherParent;
    EXPECT_EQ(gtCalculatorHelperFactory->makeCalculatorHelper(
                  GT_CLASSNAME(CalcHelper), QStringLiteral("helper"),
                  otherParent),
              nullptr);

    // ambiguous: two children with same name and class
    CalcHelperParent ambiguousParent;
    ASSERT_NE(gtCalculatorHelperFactory->makeCalculatorHelper(
                  GT_CLASSNAME(CalcHelper), QStringLiteral("helper"),
                  ambiguousParent),
              nullptr);
    auto* duplicate = new CalcHelper;
    duplicate->setObjectName(QStringLiteral("helper"));
    ASSERT_TRUE(ambiguousParent.appendChild(duplicate));

    EXPECT_EQ(gtCalculatorHelperFactory->makeCalculatorHelper(
                  GT_CLASSNAME(CalcHelper), QStringLiteral("helper"),
                  ambiguousParent),
              nullptr);
}

#include "test_gt_calculatorhelperfactory.moc"
