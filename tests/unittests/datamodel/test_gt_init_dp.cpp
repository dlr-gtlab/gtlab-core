/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include <gt_qtutilities.h>

#include <gtest/gtest.h>

#include <QVariant>

TEST(DataProcessorInit, BasicTypesRegistered)
{
    EXPECT_TRUE(gt::metaTypeNameIsRegistered("QList<double>"));
    EXPECT_TRUE(gt::metaTypeNameIsRegistered("QList<int>"));
    EXPECT_TRUE(gt::metaTypeNameIsRegistered("QList<bool>"));
    EXPECT_TRUE(gt::metaTypeNameIsRegistered("QList<QPointF>"));

    EXPECT_TRUE(gt::metaTypeNameIsRegistered("QVector<double>"));
    EXPECT_TRUE(gt::metaTypeNameIsRegistered("QVector<int>"));
    EXPECT_TRUE(gt::metaTypeNameIsRegistered("QVector<bool>"));
    EXPECT_TRUE(gt::metaTypeNameIsRegistered("QVector<QPointF>"));

    EXPECT_TRUE(gt::metaTypeNameIsRegistered("QStringList"));

    EXPECT_FALSE(gt::metaTypeNameIsRegistered("_MyReallyInvalidType_"));
}
