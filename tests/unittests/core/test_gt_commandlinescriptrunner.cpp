/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_commandlinetemplatefinder
 *
 *  Created on: 04.03.2026
 *  Author: Matthias Schuff (SG-VTM)
 *  Tel.: +49 821 319874 2128
 */

#include "gtest/gtest.h"


#include <QStringList>
#include <QMap>
#include <QDir>

#include "gt_commandlinescriptrunner.h"




class TestCommandlineScriptRunner : public GtCommandlineScriptRunner
{
public:
    TestCommandlineScriptRunner() : GtCommandlineScriptRunner("")
    {

    }

};

/// This is a test fixture that does a init for each test
class TestGtCommandlineScriptRunner : public ::testing::Test
{
protected:
    void SetUp() override
    {
        QDir currdir = QFileInfo(__FILE__).absoluteDir();
        QDir tmpdir = QDir(currdir.absoluteFilePath("tmp")); // better place for a temp working directory?
        QDir datadir = QDir(currdir.absoluteFilePath("data"));
        m_templatedir = datadir.absoluteFilePath("commandlinetemplates");
        m_tmprundir = tmpdir.absoluteFilePath("commandlinetemplates");
        m_machine = "gen-linux";


        QDir(m_tmprundir).removeRecursively();
        if (!QDir(m_tmprundir).exists())
        {
            QDir(m_tmprundir).mkpath(".");
        }


        m_runner = new TestCommandlineScriptRunner();
#ifdef Q_OS_WIN
        m_runner->setShell("cmd.exe");
        m_machine  = "gen-win";

        // TODO: Windows treatment
#else
        m_runner->setShell("/bin/bash");
#endif

        m_runner->setExecutionDir(m_tmprundir);
    }


    void TearDown() override
    {
        delete m_runner;
    }


    GtCommandlineScriptRunner* m_runner;
    QString m_templatedir;
    QString m_tmprundir;
    QString m_machine;

};

TEST_F(TestGtCommandlineScriptRunner, run0)
{
    QString runfile = QDir(m_templatedir).absoluteFilePath(m_machine+"/errreturntool/0.0.0/ret_a_zero");
    int ret = m_runner->run(runfile);
    EXPECT_EQ(ret, 0);
    EXPECT_EQ(m_runner->exitCode(), 0);
    EXPECT_EQ(m_runner->isCrashed(), false);

}

TEST_F(TestGtCommandlineScriptRunner, run1)
{
    QString runfile = QDir(m_templatedir).absoluteFilePath(m_machine+"/errreturntool/0.0.0/ret_a_one");
    int ret = m_runner->run(runfile);
    EXPECT_EQ(ret, 1);
    EXPECT_EQ(m_runner->exitCode(), 1);
    EXPECT_EQ(m_runner->isCrashed(), false);

}

TEST_F(TestGtCommandlineScriptRunner, run2)
{
    QString runfile = QDir(m_templatedir).absoluteFilePath(m_machine+"/errreturntool/0.0.0/ret_a_two");
    int ret = m_runner->run(runfile);
    EXPECT_EQ(ret, 2);
    EXPECT_EQ(m_runner->exitCode(), 2);
    EXPECT_EQ(m_runner->isCrashed(), false);

}

TEST_F(TestGtCommandlineScriptRunner, crash)
{
    QString runfile = QDir(m_templatedir).absoluteFilePath(m_machine+"/errreturntool/0.0.0/make_a_crash");
    int ret = m_runner->run(runfile);
    EXPECT_EQ(m_runner->isCrashed(), true);
}
