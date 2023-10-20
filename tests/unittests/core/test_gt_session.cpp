/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_session
 *
 *  Created on: 06.09.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDir>
#include <QDebug>

#include "gtest/gtest.h"

#include "gt_testhelper.h"

#include "gt_session.h"

#include <memory>

class TestSession : public GtSession
{
public:
    explicit TestSession(const QString& sessionPath) :
        GtSession("testsession", sessionPath),
        m_sessionPath(sessionPath)

    {}

    QString sessionFilePath() const
    {
        return m_sessionPath;
    }

    void _toJsonObject()
    {
        toJsonObject();
    }

    bool _fromJsonObject()
    {
        return fromJsonObject(sessionFilePath());
    }

private:
    QString m_sessionPath;

};

/// This is a test fixture that does a init for each test
class TestGtSession : public ::testing::Test
{
public:
    static char* argv;

protected:
    void SetUp() override
    {
        qDebug() << "TestGtSession::SetUp";
        auto dir  = gtTestHelper->newTempDir();
        QString sessionPath = dir.absoluteFilePath("testsession.json");
        m_session = std::make_unique<TestSession>(sessionPath);

    }
    std::unique_ptr<TestSession> m_session;

};

TEST_F(TestGtSession, instance)
{
    ASSERT_FALSE(m_session->isValid());
//    ASSERT_TRUE(m_session->model() == nullptr);
}

TEST_F(TestGtSession, roamingPath)
{
    ASSERT_FALSE(GtSession::roamingPath().isEmpty());
}

TEST_F(TestGtSession, sessionFilePath)
{
    ASSERT_FALSE(m_session->sessionFilePath().isEmpty());
}

TEST_F(TestGtSession, fromJsonObject)
{
    ASSERT_FALSE(m_session->_fromJsonObject());
}
