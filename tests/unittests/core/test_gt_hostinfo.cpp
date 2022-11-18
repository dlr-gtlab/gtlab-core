/* GTlab - Gas Turbine laboratory
 * copyright 2009-2022 by DLR
 *
 *  Created on: 4.11.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include <QDebug>

#include "gtest/gtest.h"

#include "gt_hostinfo.h"

class TestGtHostInfo : public testing::Test
{

};

TEST_F(TestGtHostInfo, localhost)
{
    // timeout of 0 = should resolve immediatly
    QHostAddress address = GtHostInfo::lookupHost("localhost", 0);

    EXPECT_FALSE(address.isNull());
    EXPECT_TRUE(address.isLoopback());
    EXPECT_TRUE(address.isEqual(QHostAddress::LocalHost));

    EXPECT_TRUE(address.isEqual(GtHostInfo::lookupHost(
                                    QHostInfo::localHostName(), 0)));

    EXPECT_TRUE(address.isEqual(GtHostInfo::lookupHost("127.0.0.1", 0)));
}

TEST_F(TestGtHostInfo, remote)
{
    // this requires a lookup
    QHostAddress address = GtHostInfo::lookupHost("www.dlr.de");

    EXPECT_FALSE(address.isNull());
}
