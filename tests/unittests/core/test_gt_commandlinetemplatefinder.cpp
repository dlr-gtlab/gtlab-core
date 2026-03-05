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

#include "gt_commandlinetemplatefinder.h"



class TestCommandlineTemplateFinder : public GtCommandlineTemplateFinder
{
public:
    TestCommandlineTemplateFinder(const QString &templatedir) : GtCommandlineTemplateFinder()
    {

        this->setTemplateSearchPath(templatedir);
        this->setMachine("gen-linux");
        this->setOsType("unix");
        this->setDefaultShell("/bin/bash");


        std::cout << "Os:" << this->osType().toStdString() << std::endl;
        std::cout << "Search path:" << this->templateSearchPath().toStdString() << std::endl;
        std::cout << "Machine:" << this->machine().toStdString() << std::endl;
        std::cout << "DefaultShell:" << this->defaultShell().toStdString() << std::endl;
    }

};

/// This is a test fixture that does a init for each test
class TestGtCommandlineTemplateFinder : public ::testing::Test
{
protected:
    void SetUp() override
    {
        QDir currdir = QFileInfo(__FILE__).absoluteDir();
        QDir datadir = QDir(currdir.absoluteFilePath("data"));
        m_templatedir = datadir.absoluteFilePath("commandlinetemplates");



        m_finder = new TestCommandlineTemplateFinder(m_templatedir);

    }

    QString assembleToolDir(const QString &tool, const QString &version, const QString &templatename)
    {
        QString p = m_templatedir;
        p = QDir(p).absoluteFilePath(m_finder->machine());
        p = QDir(p).absoluteFilePath(tool);
        p = QDir(p).absoluteFilePath(version);
        p = QDir(p).absoluteFilePath(templatename);
        return p;
    }

    void TearDown() override
    {
        delete m_finder;
    }



    GtCommandlineTemplateFinder* m_finder;
    QString m_templatedir;

};

TEST_F(TestGtCommandlineTemplateFinder, instance)
{
    ASSERT_TRUE(m_finder != nullptr);

    ASSERT_TRUE(m_finder->templateSearchPath() == m_templatedir);
    ASSERT_TRUE(m_finder->machine() == "gen-linux");
    ASSERT_TRUE(m_finder->osType() == "unix");
    ASSERT_TRUE(m_finder->defaultShell() == "/bin/bash");
}

TEST_F(TestGtCommandlineTemplateFinder, findtoolversion1)
{
    ASSERT_TRUE(m_finder->hasToolVersion("hellotool", "1.0.0-en"));
    ASSERT_TRUE(m_finder->hasToolVersion("hellotool", "1.0.0-fr"));
    ASSERT_FALSE(m_finder->hasToolVersion("hellotool", "1.0.0-de"));
    ASSERT_FALSE(m_finder->hasToolVersion("goodbyetool", "0.0.0"));
}

TEST_F(TestGtCommandlineTemplateFinder, wrongmachine)
{
    ASSERT_TRUE(m_finder->hasToolVersion("hellotool", "1.0.0-en"));
    m_finder->setMachine("wrong-linux");
    ASSERT_FALSE(m_finder->hasToolVersion("hellotool", "1.0.0-en"));
}


TEST_F(TestGtCommandlineTemplateFinder, hastemplate1)
{

    ASSERT_TRUE(m_finder->hasTemplate("hellotool", "1.0.0-en", "helloworld"));
    ASSERT_TRUE(m_finder->hasTemplate("hellotool", "1.0.0-en", "helloperson"));
    ASSERT_TRUE(m_finder->hasTemplate("hellotool", "1.0.0-fr", "helloworld"));
    ASSERT_TRUE(m_finder->hasTemplate("hellotool", "1.0.0-fr", "helloperson"));

    ASSERT_FALSE(m_finder->hasTemplate("hellotool", "1.0.0-en", "xyz"));
    ASSERT_FALSE(m_finder->hasTemplate("hellotool", "1.0.0-fr", "xyz"));

    ASSERT_FALSE(m_finder->hasTemplate("hellotool1", "1.0.0-en", "helloworld"));
}

TEST_F(TestGtCommandlineTemplateFinder, gettemplate1)
{

    auto assertGetTemplate = [this](const QStringList x, const int err_expected)
    {

        int err = 0;
        QString p = m_finder->getTemplate(x[0], x[1], x[2], &err);
        QString p_expected = this->assembleToolDir(x[0], x[1], x[2]);

        if (err_expected!=0){
            p_expected = "";
        }

        ASSERT_EQ(p, p_expected);
        ASSERT_EQ(err, err_expected);
    };


    QStringList x;
    int err_expected = 0;

    x.clear();
    x << "hellotool" << "1.0.0-en" << "helloworld";
    assertGetTemplate(x, 0);

    x.clear();
    x << "hellotool" << "1.0.0-en" << "helloperson";
    assertGetTemplate(x, 0);

    x.clear();
    x << "hellotool" << "1.0.0-en" << "xyz";
    assertGetTemplate(x, 1);

    x.clear();
    x << "hellotool1" << "1.0.0-en" << "helloperson";
    assertGetTemplate(x, 1);

    x.clear();
    x << "hellotool" << "1.0.0-de" << "helloperson";
    assertGetTemplate(x, 1);

}
