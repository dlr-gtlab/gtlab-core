/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_projectprovider
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.02.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"

#include <QDir>
#include <QCoreApplication>

#include "gt_projectprovider.h"

/// This is a test fixture that does a init for each test
class TestGtProjectProvider : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_testPath = QString("..") + QDir::separator() + QString("tests")
                     + QDir::separator() + QString("unittests")
                     + QDir::separator() + QString("resources")
                     + QDir::separator() + QString("Unit Test Project");
    }

    virtual void TearDown()
    {

    }

    GtProjectProvider m_provider;

    QString m_testPath;
};

TEST_F(TestGtProjectProvider, constructors)
{
    // default constructor
    ASSERT_TRUE(m_provider.projectName().isEmpty());
    ASSERT_TRUE(m_provider.projectPath().isEmpty());
    ASSERT_TRUE(m_provider.projectModules().isEmpty());

//    // filename constructor
//    QString filename = m_testPath + QDir::separator()
//                       + QString("project.gtlab");
//    GtProjectProvider filenameProvider(filename);
}

TEST_F(TestGtProjectProvider, projectNameHandling)
{
    // check initial string
    ASSERT_TRUE(m_provider.projectName().isEmpty());

    // non-empty project name
    m_provider.setProjectName("Test Project");
    ASSERT_STREQ(m_provider.projectName().toStdString().c_str(),
                 "Test Project");

    // empty project name
    m_provider.setProjectName(QString());
    ASSERT_FALSE(m_provider.projectName().isEmpty());
    ASSERT_STREQ(m_provider.projectName().toStdString().c_str(),
                 "Test Project");
}

TEST_F(TestGtProjectProvider, projectPathHandling)
{
    // check initial string
    ASSERT_TRUE(m_provider.projectPath().isEmpty());

    // non-empty project path
    m_provider.setProjectPath("D:\test");
    ASSERT_STREQ(m_provider.projectPath().toStdString().c_str(),
                 "D:\test");

    // empty project path
    m_provider.setProjectPath(QString());
    ASSERT_FALSE(m_provider.projectPath().isEmpty());
    ASSERT_STREQ(m_provider.projectPath().toStdString().c_str(),
                 "D:\test");
}

TEST_F(TestGtProjectProvider, projectModulesHandling)
{
    // check initial string
    ASSERT_TRUE(m_provider.projectModules().isEmpty());

    // non-empty project modules
    QStringList modStr;
    modStr << "Module 1" << "Module 2" << "Module 3";

    m_provider.setProjectModules(modStr);
    QStringList tmpStr = m_provider.projectModules();
    ASSERT_EQ(modStr.size(), tmpStr.size());

    for (int i = 0; i < tmpStr.size(); i++)
    {
        ASSERT_STREQ(tmpStr[i].toStdString().c_str(),
                     modStr[i].toStdString().c_str());
    }

    // empty project modules
    m_provider.setProjectModules(QStringList());
    ASSERT_TRUE(m_provider.projectModules().isEmpty());
}

TEST_F(TestGtProjectProvider, moduleFilename)
{
    const QString id = "test";
    const QString file = QString("D:\test") + QDir::separator() + id +
                         QString(".gtmod");

    // check initial string
    ASSERT_TRUE(m_provider.projectModules().isEmpty());

    // module file with empty path
    ASSERT_TRUE(m_provider.moduleFilename(id).isEmpty());

    // module file with empty path and empty id
    ASSERT_TRUE(m_provider.moduleFilename(QString()).isEmpty());

    m_provider.setProjectPath("D:\test");

    // module file with empty id
    ASSERT_TRUE(m_provider.moduleFilename(QString()).isEmpty());

    // check filename
    QString filename = m_provider.moduleFilename(id);

    ASSERT_STREQ(filename.toStdString().c_str(),
                 file.toStdString().c_str());
}
