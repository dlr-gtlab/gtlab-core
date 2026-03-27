/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_environment.cpp
 */

#include "gtest/gtest.h"

#include <QTemporaryDir>
#include <QUuid>

#include "gt_environment.h"

namespace
{
QString uniqueVar(const char* suffix)
{
    return QStringLiteral("unittest_env_%1_%2")
        .arg(QString::fromLatin1(suffix), QUuid::createUuid().toString(QUuid::WithoutBraces));
}
}

class TestGtEnvironment : public ::testing::Test
{
protected:
    GtEnvironment* env = gtEnvironment;
};

TEST_F(TestGtEnvironment, addEnvironmentVariableCreatesUndefinedVariableOnce)
{
    const auto var = uniqueVar("undefined");

    EXPECT_TRUE(env->addEnvironmentVariable(var));
    EXPECT_TRUE(env->environmentVariableExists(var));
    EXPECT_TRUE(env->isUndefined(var));
    EXPECT_FALSE(env->addEnvironmentVariable(var));
}

TEST_F(TestGtEnvironment, setValueTransitionsBetweenUndefinedAndDefined)
{
    const auto var = uniqueVar("value");

    ASSERT_TRUE(env->addEnvironmentVariable(var));
    EXPECT_TRUE(env->isUndefined(var));

    env->setValue(var, 42);

    EXPECT_FALSE(env->isUndefined(var));
    EXPECT_EQ(env->value(var), QVariant(42));

    env->setValue(var, QVariant());

    EXPECT_TRUE(env->isUndefined(var));
    EXPECT_FALSE(env->value(var).isValid());
}

TEST_F(TestGtEnvironment, addEnvironmentVariablesAndVarIdsReturnSortedUniqueIds)
{
    const auto varA = uniqueVar("b");
    const auto varB = uniqueVar("a");

    env->addEnvironmentVariables({varA, varB, varA});

    const auto ids = env->varIds();

    EXPECT_TRUE(ids.contains(varA));
    EXPECT_TRUE(ids.contains(varB));
    EXPECT_EQ(ids.count(varA), 1);
    EXPECT_EQ(ids.count(varB), 1);
    EXPECT_LT(ids.indexOf(varB), ids.indexOf(varA));
}

TEST_F(TestGtEnvironment, setValueForUnknownVariableDoesNothing)
{
    const auto var = uniqueVar("missing");

    EXPECT_FALSE(env->environmentVariableExists(var));

    env->setValue(var, 1);

    EXPECT_FALSE(env->environmentVariableExists(var));
    EXPECT_FALSE(env->value(var).isValid());
}

TEST_F(TestGtEnvironment, saveAndLoadEnvironmentRoundTripsDefinedValues)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());

    const auto varA = uniqueVar("save_a");
    const auto varB = uniqueVar("save_b");

    env->setRoamingDir(dir.path());
    ASSERT_TRUE(env->addEnvironmentVariable(varA));
    ASSERT_TRUE(env->addEnvironmentVariable(varB));
    env->setValue(varA, QStringLiteral("alpha"));
    env->setValue(varB, 7);
    env->saveEnvironment();

    env->setValue(varA, QStringLiteral("changed"));
    env->setValue(varB, 9);

    env->loadEnvironment();

    EXPECT_EQ(env->value(varA), QVariant(QStringLiteral("alpha")));
    EXPECT_EQ(env->value(varB), QVariant(7));
}
