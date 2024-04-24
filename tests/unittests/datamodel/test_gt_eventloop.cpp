/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 7.10.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "gtest/gtest.h"

#include "gt_eventloop.h"

#include <QTimer>

static const int timeout = 1 * 1000;
static const int no_timeout = 0;

class TestGtEventLoop :
        public ::testing::TestWithParam<int>
{ };


INSTANTIATE_TEST_SUITE_P(
    TestGtEventLoop,
    TestGtEventLoop,
    ::testing::Values(timeout, no_timeout));


TEST_P(TestGtEventLoop, timeout)
{
    GtEventLoop future{GetParam()};

    // future is not connected -> timeout
    EXPECT_TRUE(future.exec() == GtEventLoop::Failed);
}

TEST_P(TestGtEventLoop, success)
{
    QTimer timer;
    GtEventLoop loop{2 * (GetParam()+1)};

    loop.connectSuccess(&timer, &QTimer::timeout);

    timer.start(GetParam());

    // future should wait until signal is emitted
    EXPECT_TRUE(loop.exec() == GtEventLoop::Success);
}

TEST_P(TestGtEventLoop, preliminaryState)
{
    GtEventLoop loop{GetParam()};

    // emitted before even loop
    emit loop.success();

    // future should finish right away
    EXPECT_TRUE(loop.exec() == GtEventLoop::Success);
}

TEST_P(TestGtEventLoop, multiplePreliminaryState)
{
    GtEventLoop loop{GetParam()};

    // emitted before even loop
    emit loop.abort();
    emit loop.success();

    // future should finish right away
    EXPECT_TRUE(loop.exec() == GtEventLoop::Success);
    // now loop will run into timeout
    EXPECT_TRUE(loop.exec() == GtEventLoop::Failed);
}
