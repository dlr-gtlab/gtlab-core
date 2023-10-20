/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include <gtest/gtest.h>

#include <QVariant>

TEST(DataProcessorInit, BasicTypesRegistered)
{
    EXPECT_NE(QVariant::Invalid, QVariant::nameToType("QList<double>"));
    EXPECT_NE(QVariant::Invalid, QVariant::nameToType("QList<int>"));
    EXPECT_NE(QVariant::Invalid, QVariant::nameToType("QList<bool>"));
    EXPECT_NE(QVariant::Invalid, QVariant::nameToType("QList<QPointF>"));

    EXPECT_NE(QVariant::Invalid, QVariant::nameToType("QVector<double>"));
    EXPECT_NE(QVariant::Invalid, QVariant::nameToType("QVector<int>"));
    EXPECT_NE(QVariant::Invalid, QVariant::nameToType("QVector<bool>"));
    EXPECT_NE(QVariant::Invalid, QVariant::nameToType("QVector<QPointF>"));

    EXPECT_NE(QVariant::Invalid, QVariant::nameToType("QStringList"));
}
