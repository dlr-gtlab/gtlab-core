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
#include "gt_object.h"

#include <QTimer>

static const int timeout = 1 * 1000;

class TestGtEventLoop : public testing::Test {};

TEST_F(TestGtEventLoop, timeout)
{
    GtEventLoop future{timeout};

    // future is not connected -> timeout
    EXPECT_TRUE(future.exec() == GtEventLoop::Failed);
}

TEST_F(TestGtEventLoop, success)
{
    QTimer timer;
    GtEventLoop loop{2 * timeout};

    loop.connectSuccess(&timer, &QTimer::timeout);

    timer.start(timeout);

    // future should wait until signal is emitted
    EXPECT_TRUE(loop.exec() == GtEventLoop::Success);
}

TEST_F(TestGtEventLoop, preliminaryState)
{
    GtEventLoop loop{timeout};

    // emitted before even loop
    emit loop.success();

    // future should finish righ away
    EXPECT_TRUE(loop.exec() == GtEventLoop::Success);
}

TEST_F(TestGtEventLoop, multiplePreliminaryState)
{
    GtEventLoop loop{timeout};

    // emitted before even loop
    emit loop.abort();
    emit loop.success();

    // future should finish righ away
    EXPECT_TRUE(loop.exec() == GtEventLoop::Success);
    // now loop will run into timeout
    EXPECT_TRUE(loop.exec() == GtEventLoop::Failed);
}
