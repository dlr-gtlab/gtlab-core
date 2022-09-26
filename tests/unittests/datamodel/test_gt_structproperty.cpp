/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_structproperty
 * copyright 2021 by DLR
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */

#include "gtest/gtest.h"

#include <QVariant>

#include "gt_structproperty.h"

#include "gt_propertystructcontainer.h"
#include "gt_doubleproperty.h"
#include "gt_objectmemento.h"
#include "gt_xmlutilities.h"

#include "gt_object.h"

struct TestObject : public GtObject
{
    TestObject() :
        GtObject()
    {

        GtPropertyStructDefinition envVarStruct("EnvironmentVarsStruct");
        envVarStruct.defineMember("name", "GtStringProperty", "");
        envVarStruct.defineMember("value", "GtStringProperty", "");

        environmentVars.registerAllowedType(envVarStruct);

        registerPropertyStructContainer(environmentVars);
    }

    void addEnvironmentVar(QString name, QString value)
    {
        auto* vars = findDynamicSizeProperty("environmentVars");

        auto currentSize = vars->size();

        auto& var = vars->newEntry(QString("[%1]").arg(currentSize),
                                   "EnvironmentVarsStruct");
        var.setMemberVal("name", name);
        var.setMemberVal("value", value);
    }

    GtPropertyStructContainer environmentVars{"environmentVars"};
};


/// This is a test fixture
class TestGtStructProperty : public ::testing::Test
{
};


TEST_F(TestGtStructProperty, buildPoint3d)
{
    GtPropertyStructDefinition point3dClz("point_3d");

    auto makeDoubleProperty = [](double value) {
        return [=](const QString& id) {
            auto p = new GtDoubleProperty(id, id);
            p->setValueFromVariant(value, "");
            return p;
        };
    };

    point3dClz.defineMember("x", makeDoubleProperty(1.0));
    point3dClz.defineMember("y", makeDoubleProperty(2.0));

    // lets define z via the property factory
    point3dClz.defineMember("z", "GtDoubleProperty", 3.0);

    auto p1 = point3dClz.newInstance("p1");

    EXPECT_EQ("point_3d", p1->typeName().toStdString());
    EXPECT_EQ("p1", p1->ident().toStdString());


    EXPECT_TRUE(p1->findProperty("_this_does_not_exist_") == nullptr);

    auto props = p1->properties();
    EXPECT_EQ(3, props.size());
    EXPECT_EQ("x", props[0]->ident().toStdString());
    EXPECT_EQ("y", props[1]->ident().toStdString());
    EXPECT_EQ("z", props[2]->ident().toStdString());

    auto px = p1->findProperty("x");
    auto py = p1->findProperty("y");
    auto pz = p1->findProperty("z");

    ASSERT_TRUE(px != nullptr);
    ASSERT_TRUE(py != nullptr);
    ASSERT_TRUE(pz != nullptr);

    EXPECT_EQ("x", px->ident().toStdString());
    EXPECT_EQ("y", py->ident().toStdString());
    EXPECT_EQ("z", pz->ident().toStdString());

    // check values
    EXPECT_EQ(1.0, px->value<double>());
    EXPECT_EQ(2.0, py->value<double>());
    EXPECT_EQ(3.0, pz->value<double>());
}

TEST_F(TestGtStructProperty, findDynamicSizeProperty)
{
    TestObject obj;

    ASSERT_TRUE(obj.findDynamicSizeProperty("environmentVars") != nullptr);

    ASSERT_TRUE(obj.findDynamicSizeProperty("_thisDoesNotExist_") == nullptr);
}

TEST_F(TestGtStructProperty, checkPropertySize)
{
    TestObject obj;

    EXPECT_EQ(0, obj.environmentVars.size());

    auto* props = obj.findDynamicSizeProperty("environmentVars");
    ASSERT_TRUE(props != nullptr);

    EXPECT_EQ(0, props->size());

    obj.addEnvironmentVar("PATH", "/usr/bin");

    EXPECT_EQ(1, props->size());
}

TEST_F(TestGtStructProperty, checkContent)
{
    TestObject obj;
    obj.addEnvironmentVar("PATH", "/usr/bin");

    auto* props = obj.findDynamicSizeProperty("environmentVars");

    ASSERT_EQ(1, props->size());

    auto& props0 = props->at(0);
    EXPECT_EQ(QString("[0]"), props0.ident());

    EXPECT_EQ(QString("PATH"), props0.getMemberVal<QString>("name"));
    EXPECT_EQ(QString("/usr/bin"), props0.getMemberVal<QString>("value"));

    bool okay = true;
    props0.getMemberVal<QString>("_not_existentent_", &okay);
    EXPECT_FALSE(okay);

    EXPECT_FALSE(props0.setMemberVal("_not_existentent_", "Welt"));
    EXPECT_TRUE(props0.setMemberVal("value", "/usr/local/bin"));

    okay = true;
    props0.getMemberVal<int>("value", &okay);
    EXPECT_FALSE(okay);
}

TEST_F(TestGtStructProperty, write)
{
    TestObject obj;
    obj.addEnvironmentVar("PATH", "/usr/bin");
    obj.addEnvironmentVar("LD_DEBUG", "1");

    auto memento = obj.toMemento(false);
    QDomDocument doc("");
    auto docElem = memento.documentElement();

    doc.appendChild(docElem);

    GtXmlUtilities::writeDomDocumentToFile("test.xml", doc);

    QDomElement container = docElem.firstChildElement("property_container");
    ASSERT_FALSE(container.isNull());

    EXPECT_EQ(QString("environmentVars").toStdString(),
              container.attribute("name").toStdString());

    // There are 2 variables defined
    auto prop1 = container.firstChildElement("property");
    ASSERT_FALSE(prop1.isNull());

    EXPECT_EQ(QString("EnvironmentVarsStruct"), prop1.attribute("type"));

    auto name1 = prop1.firstChildElement();
    EXPECT_EQ("property", name1.tagName());
    EXPECT_EQ("name", name1.attribute("name"));
    EXPECT_EQ("QString", name1.attribute("type"));

    EXPECT_EQ(QString("PATH").toStdString(),
              name1.text().toStdString());

    auto value1 = name1.nextSiblingElement();
    EXPECT_EQ("property", value1.tagName());
    EXPECT_EQ("value", value1.attribute("name"));
    EXPECT_EQ("QString", value1.attribute("type"));


    EXPECT_EQ(QString("/usr/bin").toStdString(),
              value1.text().toStdString());

    auto prop2 = prop1.nextSiblingElement("property");
    ASSERT_FALSE(prop2.isNull());


    auto name2 = prop2.firstChildElement();
    EXPECT_EQ("property", name2.tagName());
    EXPECT_EQ("name", name2.attribute("name"));
    EXPECT_EQ("QString", name2.attribute("type"));
    EXPECT_EQ("LD_DEBUG", name2.text());

    auto value2 = name2.nextSiblingElement();
    EXPECT_EQ("property", value2.tagName());
    EXPECT_EQ("value", value2.attribute("name"));
    EXPECT_EQ("QString", value2.attribute("type"));


    EXPECT_EQ(QString("1").toStdString(),
              value2.text().toStdString());

    auto prop3 = prop2.nextSiblingElement("property");
    ASSERT_TRUE(prop3.isNull());

}
