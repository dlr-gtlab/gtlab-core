/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: teset_gt_accessdata
 *
 *  Created on: 18.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include <QUrl>

#include "gt_accessdata.h"

/// This is a test fixture that does a init for each test
class TestGtAccessData : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        host = "198.167.0.1";
        port = 22;
        user = "test_user";
        pw = "my_password";

        cons1 = new GtAccessData(host, port, user, pw);
        cons2 = new GtAccessData(host, port);
        cons3 = new GtAccessData(host, user, pw);
        cons4 = new GtAccessData(host);
    }

    virtual void TearDown()
    {
        delete cons1;
        delete cons2;
        delete cons3;
        delete cons4;
    }

    GtAccessData* cons1;
    GtAccessData* cons2;
    GtAccessData* cons3;
    GtAccessData* cons4;

    QString host;
    int port;
    QString user;
    QString pw;

};

TEST_F(TestGtAccessData, host)
{
    // constructor 1
    ASSERT_STREQ(cons1->host().toStdString().c_str(),
                 host.toStdString().c_str());

    // constructor 2
    ASSERT_STREQ(cons2->host().toStdString().c_str(),
                 host.toStdString().c_str());

    // constructor 3
    ASSERT_STREQ(cons3->host().toStdString().c_str(),
                 host.toStdString().c_str());

    // constructor 4
    ASSERT_STREQ(cons4->host().toStdString().c_str(),
                 host.toStdString().c_str());
}

TEST_F(TestGtAccessData, port)
{
    // constructor 1
    ASSERT_EQ(cons1->port(), port);

    // constructor 2
    ASSERT_EQ(cons2->port(), port);

    // constructor 3
    ASSERT_NE(cons3->port(), port);
    ASSERT_EQ(cons3->port(), -1);

    // constructor 4
    ASSERT_NE(cons4->port(), port);
    ASSERT_EQ(cons4->port(), -1);
}

TEST_F(TestGtAccessData, user)
{
    // constructor 1
    ASSERT_STREQ(cons1->user().toStdString().c_str(),
                 user.toStdString().c_str());

    // constructor 2
    ASSERT_STRNE(cons2->user().toStdString().c_str(),
                 user.toStdString().c_str());
    ASSERT_TRUE(cons2->user().isEmpty());

    // constructor 3
    ASSERT_STREQ(cons3->user().toStdString().c_str(),
                 user.toStdString().c_str());

    // constructor 4
    ASSERT_STRNE(cons4->user().toStdString().c_str(),
                 user.toStdString().c_str());
    ASSERT_TRUE(cons4->user().isEmpty());
}

TEST_F(TestGtAccessData, password)
{
    // constructor 1
    ASSERT_STREQ(cons1->password().toStdString().c_str(),
                 pw.toStdString().c_str());

    // constructor 2
    ASSERT_STRNE(cons2->password().toStdString().c_str(),
                 pw.toStdString().c_str());
    ASSERT_TRUE(cons2->password().isEmpty());

    // constructor 3
    ASSERT_STREQ(cons3->password().toStdString().c_str(),
                 pw.toStdString().c_str());

    // constructor 4
    ASSERT_STRNE(cons4->password().toStdString().c_str(),
                 pw.toStdString().c_str());
    ASSERT_TRUE(cons4->password().isEmpty());
}

TEST_F(TestGtAccessData, isEmpty)
{
    GtAccessData td("");

    ASSERT_TRUE(td.isEmpty());
}

TEST_F(TestGtAccessData, data)
{
    // constructor 1
    QUrl data1 = cons1->data();
    ASSERT_FALSE(data1.isEmpty());
    ASSERT_EQ(data1.port(), port);

    // constructor 2
    QUrl data2 = cons2->data();
    ASSERT_FALSE(data2.isEmpty());
    ASSERT_EQ(data2.port(), port);

    // constructor 3
    QUrl data3 = cons3->data();
    ASSERT_FALSE(data3.isEmpty());
    ASSERT_NE(data3.port(), port);
    ASSERT_EQ(data3.port(), -1);

    // constructor 4
    QUrl data4 = cons4->data();
    ASSERT_FALSE(data4.isEmpty());
    ASSERT_NE(data4.port(), port);
    ASSERT_EQ(data4.port(), -1);

    // constructor 5
    GtAccessData td("");
    QUrl data5 = td.data();
    ASSERT_TRUE(data5.isEmpty());
}
