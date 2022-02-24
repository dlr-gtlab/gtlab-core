/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_session
 * copyright 2009-2016 by DLR
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

class TestSession : public GtSession
{
public:
    TestSession() = default;

    QString sessionFilePath() override
    {
        return m_tmpDir.absoluteFilePath(objectName() + ".json");
    }

    void _toJsonObject()
    {
        toJsonObject();
    }

    bool _fromJsonObject()
    {
        return fromJsonObject();
    }

private:
    QDir m_tmpDir{gtTestHelper->newTempDir()};

};

/// This is a test fixture that does a init for each test
class TestGtSession : public ::testing::Test
{
public:
    static char* argv;

protected:
    virtual void SetUp()
    {
        qDebug() << "TestGtSession::SetUp";

        m_session = new TestSession;
        m_session->setObjectName("Test Session");
    }

    virtual void TearDown()
    {
    }

    TestSession* m_session;

};

TEST_F(TestGtSession, instance)
{
    ASSERT_FALSE(m_session->isValid());
//    ASSERT_TRUE(m_session->model() == Q_NULLPTR);
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
