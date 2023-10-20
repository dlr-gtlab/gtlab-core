/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include "internal/varianthasher.h"

#include <gtest/gtest.h>



struct TestVariantHasher : public testing::Test
{
};

Q_DECLARE_METATYPE(std::string)

namespace
{

QDataStream& operator<<(QDataStream& stream, const std::string& str)
{

    stream << str.c_str();
    return stream;
}

// Just a dummy operator to make qRegisterMetaTypeStreamOperators work
QDataStream& operator>>(QDataStream& stream, std::string& )
{

    return stream;
}
}



/**
 * Check, that the variant hasher always computes
 * the same hash for same data
 */
TEST_F(TestVariantHasher, testIssue386)
{
    gt::detail::VariantHasher hasher;


    qRegisterMetaTypeStreamOperators<std::string>("std::string");

    QCryptographicHash r1(QCryptographicHash::Sha256);
    QCryptographicHash r2(QCryptographicHash::Sha256);
    QCryptographicHash a1(QCryptographicHash::Sha256);
    QCryptographicHash a2(QCryptographicHash::Sha256);

    QVariant aStdStringVariant;
    aStdStringVariant.setValue(
                std::string("aLorem ipsum dolor sit amet, consetetur "
                            "sadipscing elitr, sed diam nonumy eirmod "
                            "tempor invidunt ut labore et"));
    hasher.addToHash(r2, static_cast<unsigned int>(2));
    hasher.addToHash(r1, aStdStringVariant);


    hasher.addToHash(a1, aStdStringVariant);
    hasher.addToHash(a2, static_cast<unsigned int>(2));

    ASSERT_EQ(r1.result().toStdString(), a1.result().toStdString());
    ASSERT_EQ(r2.result().toStdString(), a2.result().toStdString());
}
