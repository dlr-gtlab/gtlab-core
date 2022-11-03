#include "gtest/gtest.h"
#include "gt_xmlutilities.h"

#include <QString>
#include <QDomDocument>
#include <QDebug>



class  TestXmlUtilities : public testing::Test
{
public:
    void SetUp() override
    {
        QString errorMsg;
        doc.setContent(QStringLiteral(
R"(
<object class="GtProject" uuid="{e164c606-6061-4ecc-a1ea-6b80c0d8519f}" name="ModuleConverterCheck">
 <property type="QString" name="path">C:/data/ModuleConverterCheck</property>
 <objectlist>
  <object class="GtProcessData" uuid="{49d14dfe-16be-43d4-af15-f872ad4f6631}" name="Process Data"/>
  <object class="GtLabel" uuid="{5550ff0f-2a8f-432c-a68e-f62df1c5a9cc}" name="ALabel"/>
  <dummy class="GtLabel" name="Dummy"/>
  <object class="GtLabelData" uuid="{2694e08e-9208-4dc5-ba60-5eb1f106a917}" name="Label Data">
   <objectlist>
    <object class="GtLabel" uuid="{8fd7bd4d-5638-4873-948b-62abaf4e975f}" name="AnotherLabel"/>
   </objectlist>
  </object>
 </objectlist>
</object>)"), &errorMsg);
        ASSERT_TRUE(errorMsg.isEmpty());
    }

    QDomDocument doc;

};

TEST_F(TestXmlUtilities, findObjectElementsByClassName)
{
    auto root = doc.firstChildElement();
    ASSERT_TRUE(root.tagName() == "object");

    auto labels = gt::xml::findObjectElementsByClassName(root, "GtLabel");
    EXPECT_EQ(2, labels.size());
    EXPECT_EQ("ALabel", labels[0].attribute("name").toStdString());
    EXPECT_EQ("AnotherLabel", labels[1].attribute("name").toStdString());

    auto projects = gt::xml::findObjectElementsByClassName(root, "GtProject");
    EXPECT_EQ(1, projects.size());
    EXPECT_EQ("ModuleConverterCheck", projects[0].attribute("name").toStdString());
}

TEST_F(TestXmlUtilities, findObjectElementsByClassNameChangeValue)
{
    auto root = doc.firstChildElement();
    ASSERT_TRUE(root.tagName() == "object");

    auto projects = gt::xml::findObjectElementsByClassName(root, "GtProject");
    ASSERT_EQ(1, projects.size());
    projects[0].setAttribute("name", "blubb");

    auto projectsNew = gt::xml::findObjectElementsByClassName(root, "GtProject");
    ASSERT_EQ(1, projectsNew.size());
    EXPECT_EQ("blubb", projects[0].attribute("name").toStdString());
}
