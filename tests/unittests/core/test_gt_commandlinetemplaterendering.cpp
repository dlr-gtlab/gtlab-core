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

#include "gt_commandlinetemplaterendering.h"



void ExpectQStringListEq(const QStringList& actual, const QStringList& expected)
{
    ASSERT_EQ(actual.size(), expected.size())
    << "List sizes differ.";

    for (int i = 0; i < actual.size(); ++i) {
        EXPECT_EQ(actual[i], expected[i])
        << "Difference at line " << i;
    }
}


class TestCommandlineTemplateRendering : public GtCommandlineTemplateRendering
{
public:
    TestCommandlineTemplateRendering() : GtCommandlineTemplateRendering("/bin/bash")
    {

    }

};

/// This is a test fixture that does a init for each test
class TestGtCommandlineTemplateRendering : public ::testing::Test
{
protected:
    void SetUp() override
    {
        QDir currdir = QFileInfo(__FILE__).absoluteDir();
        QDir datadir = QDir(currdir.absoluteFilePath("data"));
        m_templatedir = datadir.absoluteFilePath("commandlinetemplates");

        m_render = new TestCommandlineTemplateRendering();

    }

    void TearDown() override
    {
        delete m_render;
    }


    GtCommandlineTemplateRendering* m_render;
    QString m_templatedir;

};

TEST_F(TestGtCommandlineTemplateRendering, instance)
{
    ASSERT_TRUE(m_render != nullptr);

}

TEST_F(TestGtCommandlineTemplateRendering, readTemplate1)
{
    QString filepath = QDir(m_templatedir).absoluteFilePath("gen-linux/hellotool/1.0.0-en/helloperson");
    QStringList desired = {"#!/bin/bash", "echo \"Hello, \"$PERSON\"!\""};

    ASSERT_TRUE(m_render->loadTemplateFromFile(filepath));
    ExpectQStringListEq(m_render->scriptLines(), desired);
}

TEST_F(TestGtCommandlineTemplateRendering, readTemplate2)
{
    QString filepath = QDir(m_templatedir).absoluteFilePath("gen-linux/hellotool/1.0.0-fr/helloperson");
    QStringList desired = {"#!/bin/bash", "echo \"Bonjour, \"$PERSON\"!\""};

    ASSERT_TRUE(m_render->loadTemplateFromFile(filepath));
    ExpectQStringListEq(m_render->scriptLines(), desired);
}


TEST_F(TestGtCommandlineTemplateRendering, renderTemplate1)
{
    QStringList templatelines = {"#!/bin/bash", "echo \"Hello, \"$PERSON\"!\""};
    QStringList desired = {"#!/bin/bash", "PERSON=Trundle", "echo \"Hello, \"$PERSON\"!\""};

    m_render->loadTemplateFromList(templatelines);
    m_render->updateVariables(QMap<QString, QString>{{"PERSON", "Trundle"}});
    m_render->renderTemplateToScript();

    ExpectQStringListEq(m_render->scriptLines(), desired);
}

TEST_F(TestGtCommandlineTemplateRendering, renderTemplate2)
{
    QString filepath = QDir(m_templatedir).absoluteFilePath("gen-linux/hellotool/1.0.0-fr/helloperson");
    QStringList desired = {"#!/bin/bash", "PERSON=Arthur", "echo \"Bonjour, \"$PERSON\"!\""};

    m_render->loadTemplateFromFile(filepath);
    m_render->updateVariables(QMap<QString, QString>{{"PERSON", "Arthur"}});
    m_render->renderTemplateToScript();

    ExpectQStringListEq(m_render->scriptLines(), desired);
}



TEST_F(TestGtCommandlineTemplateRendering, renderTemplate3)
{
    QStringList templatelines = {"x"};
    QStringList desired = {"$COACH = \"Berti\"", "x"};
    GtCommandlineTemplateRendering * m_render2 = new GtCommandlineTemplateRendering("powershell.exe");

    m_render2->loadTemplateFromList(templatelines);
    m_render2->updateVariables(QMap<QString, QString>{{"COACH", "Berti"}});
    m_render2->renderTemplateToScript();

    ExpectQStringListEq(m_render2->scriptLines(), desired);

    delete m_render2;
}

TEST_F(TestGtCommandlineTemplateRendering, renderTemplate4)
{
    QStringList templatelines = {"x"};
    QStringList desired = {"set ANIMAL=\"Marmelade\"", "x"};
    GtCommandlineTemplateRendering * m_render2 = new GtCommandlineTemplateRendering("cmd.exe");

    m_render2->loadTemplateFromList(templatelines);
    m_render2->updateVariables(QMap<QString, QString>{{"ANIMAL", "Marmelade"}});
    m_render2->renderTemplateToScript();

    ExpectQStringListEq(m_render2->scriptLines(), desired);

    delete m_render2;
}
