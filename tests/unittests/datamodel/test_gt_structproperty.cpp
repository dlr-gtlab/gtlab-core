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
#include "gt_propertyfactory.h"
#include "gt_doubleproperty.h"
#include "gt_objectmemento.h"
#include "gt_xmlutilities.h"
#include "gt_objectmementodiff.h"
#include "gt_objectfactory.h"


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
        setObjectName("testobject");
    }

    void addEnvironmentVar(QString name, QString value)
    {
        auto* vars = findDynamicSizeProperty("environmentVars");
        auto& var = vars->newEntry("EnvironmentVarsStruct");

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

    QDomElement container = docElem.firstChildElement("property-container");
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

TEST_F(TestGtStructProperty, readFromMemento)
{
    TestObject obj;
    obj.addEnvironmentVar("PATH", "/usr/bin");
    obj.addEnvironmentVar("LD_DEBUG", "1");

    auto memento = obj.toMemento(false);

    TestObject newObj;

    EXPECT_EQ(0, newObj.environmentVars.size());

    memento.mergeTo(newObj, *gtObjectFactory);

    // check equality of both objects by comparing the mementos
    EXPECT_TRUE(GtObjectMementoDiff(memento, newObj.toMemento()).isNull());

    ASSERT_EQ(2, newObj.environmentVars.size());

    const auto& entry0 = newObj.environmentVars[0];
    EXPECT_EQ(QString("PATH"), entry0.getMemberVal<QString>("name"));
    EXPECT_EQ(QString("/usr/bin"), entry0.getMemberVal<QString>("value"));

    const auto& entry1 = newObj.environmentVars[1];
    EXPECT_EQ(QString("LD_DEBUG"), entry1.getMemberVal<QString>("name"));
    EXPECT_EQ(QString("1"), entry1.getMemberVal<QString>("value"));
}

TEST_F(TestGtStructProperty, readMissingDynprop)
{
    TestObject obj;

    auto memento = obj.toMemento(false);
    // remove the dynamic property from the memento
    memento.dynamicSizeProperties.clear();

    TestObject newObj;
    newObj.addEnvironmentVar("PATH", "/usr/bin");
    newObj.addEnvironmentVar("LD_DEBUG", "1");

    EXPECT_EQ(2, newObj.environmentVars.size());

    // hence, the memento should not delete / change the
    // environment vars, since it was deleted from the memento
    memento.mergeTo(newObj, *gtObjectFactory);

    EXPECT_EQ(2, newObj.environmentVars.size());
}

TEST_F(TestGtStructProperty, readXmlToMemento)
{
    QString xmlstr = R"(<?xml version="1.0" encoding="UTF-8"?>
<object class="GtObject" name="testobject" uuid="{objuid}">
    <property-container name="environmentVars">
        <property name="{p1uid}" type="EnvironmentVarsStruct">
            <property name="name" type="QString">PATH</property>
            <property name="value" type="QString">/usr/bin</property>
        </property>
        <property name="{p2uid}" type="EnvironmentVarsStruct">
            <property name="name" type="QString">LD_DEBUG</property>
            <property name="value" type="QString">1</property>
        </property>
    </property-container>
</object>)";

    QDomDocument doc;
    ASSERT_TRUE(doc.setContent(xmlstr));

    GtObjectMemento m(doc.documentElement());

    EXPECT_EQ("{objuid}", m.uuid());
    EXPECT_EQ(0, m.childObjects.size());
    EXPECT_EQ(0, m.properties.size());

    ASSERT_EQ(1, m.dynamicSizeProperties.size());

    const auto& cont = m.dynamicSizeProperties[0];
    EXPECT_EQ("environmentVars", cont.name.toStdString());

    ASSERT_EQ(2, cont.childProperties.size());

    auto entry0 = cont.childProperties[0];
    EXPECT_EQ("{p1uid}", entry0.name);
    EXPECT_EQ("EnvironmentVarsStruct", entry0.dataType());

    ASSERT_EQ(2, entry0.childProperties.size());
    auto e0p0 = entry0.childProperties[0];
    EXPECT_EQ("name", e0p0.name.toStdString());
    EXPECT_EQ("QString", e0p0.dataType().toStdString());
    EXPECT_EQ("PATH", e0p0.data().toString().toStdString());

    auto e0p1 = entry0.childProperties[1];
    EXPECT_EQ("value", e0p1.name.toStdString());
    EXPECT_EQ("QString", e0p1.dataType().toStdString());
    EXPECT_EQ("/usr/bin", e0p1.data().toString().toStdString());

    auto entry1 = cont.childProperties[1];
    EXPECT_EQ("{p2uid}", entry1.name);
    EXPECT_EQ("EnvironmentVarsStruct", entry1.dataType());


    ASSERT_EQ(2, entry1.childProperties.size());
    auto e1p0 = entry1.childProperties[0];
    EXPECT_EQ("name", e1p0.name.toStdString());
    EXPECT_EQ("QString", e1p0.dataType().toStdString());
    EXPECT_EQ("LD_DEBUG", e1p0.data().toString().toStdString());

    auto e1p1 = entry1.childProperties[1];
    EXPECT_EQ("value", e1p1.name.toStdString());
    EXPECT_EQ("QString", e1p1.dataType().toStdString());
    EXPECT_EQ("1", e1p1.data().toString().toStdString());
}

TEST_F(TestGtStructProperty, mementoDiffPlausibility)
{
    TestObject obj;
    auto beforeMemento = obj.toMemento();
    auto afterMemento = obj.toMemento();

    EXPECT_TRUE(GtObjectMementoDiff(beforeMemento, afterMemento).isNull());

    obj.addEnvironmentVar("PATH", "/usr/bin");

    afterMemento = obj.toMemento();
    EXPECT_FALSE(GtObjectMementoDiff(beforeMemento, afterMemento).isNull());

}

TEST_F(TestGtStructProperty, mementoDiffElementChanged)
{    
    TestObject obj;

    // add entry
    obj.addEnvironmentVar("PATH", "/usr/bin");

    auto beforeMemento = obj.toMemento();

    // change entry
    obj.environmentVars.at(0).setMemberVal("value", "/usr/local/bin");

    auto afterMemento = obj.toMemento();

    GtObjectMementoDiff diff(beforeMemento, afterMemento);


    /* The diff should like like the following
    <object name="" uuid="{608b7f9a-d81c-453c-ac99-efc621b753b5}" class="GtObject">
     <diff-property-container-entry-change name="environmentVars" entryName="{977e7d6f-0a20-4f6c-a027-7bf7ac70cb12}">
      <diff-property-change name="value" type="QString">
       <oldVal>/usr/bin</oldVal>
       <newVal>/usr/local/bin</newVal>
      </diff-property-change>
     </diff-property-container-entry-change>
    </object>
    */

    ASSERT_FALSE(diff.isNull());

    // check diff content
    auto objectElem = diff.documentElement();
    EXPECT_TRUE(objectElem.attribute("uuid") == obj.uuid());

    auto diffElem = objectElem.firstChildElement();
    ASSERT_TRUE(diffElem.tagName() == "diff-property-container-entry-change");
    EXPECT_TRUE(diffElem.attribute("name") == "environmentVars");
    EXPECT_TRUE(diffElem.attribute("entryName") == obj.environmentVars.at(0).ident());

    // only one prop has changed
    ASSERT_EQ(1, diffElem.childNodes().size());

    auto propChangedElem = diffElem.firstChildElement();
    ASSERT_TRUE(propChangedElem.tagName() == "diff-property-change");
    ASSERT_TRUE(propChangedElem.attribute("name") == "value");
    ASSERT_TRUE(propChangedElem.attribute("type") == "QString");

    ASSERT_EQ(2, propChangedElem.childNodes().size());

    auto oldVal = propChangedElem.firstChildElement("oldVal");
    ASSERT_FALSE(oldVal.isNull());
    EXPECT_TRUE(oldVal.text() == "/usr/bin");

    auto newVal = propChangedElem.firstChildElement("newVal");
    ASSERT_FALSE(newVal.isNull());
    EXPECT_TRUE(newVal.text() == "/usr/local/bin");
}

TEST_F(TestGtStructProperty, mementoDiffElementAdded)
{
    TestObject obj;
    obj.addEnvironmentVar("PATH", "/usr/bin");
    auto beforeMemento = obj.toMemento();

    obj.addEnvironmentVar("LD_LIBRARY_PATH", "/usr/lib");
    auto afterMemento = obj.toMemento();

    GtObjectMementoDiff diff(beforeMemento, afterMemento);

    ASSERT_FALSE(diff.isNull());

    // check diff content
    auto objectElem = diff.documentElement();
    EXPECT_TRUE(objectElem.attribute("uuid") == obj.uuid());

    auto diffElem = objectElem.firstChildElement();
    ASSERT_TRUE(diffElem.tagName() == "diff-property-container-entry-add");
    EXPECT_TRUE(diffElem.attribute("name") == "environmentVars");
    EXPECT_TRUE(diffElem.attribute("index") == "1");

    // next is the new entry
    EXPECT_EQ(1, diffElem.childNodes().size());
    auto newEntry = diffElem.firstChildElement();

    EXPECT_TRUE(newEntry.tagName() == "property");
    EXPECT_TRUE(newEntry.attribute("name") == obj.environmentVars.at(1).ident());
    EXPECT_TRUE(newEntry.attribute("type") == "EnvironmentVarsStruct");

    // it should have two sub properties
    ASSERT_EQ(2, newEntry.childNodes().size());

    auto propName = newEntry.firstChildElement();
    EXPECT_TRUE(propName.tagName() == "property");
    EXPECT_TRUE(propName.attribute("name") == "name");
    EXPECT_TRUE(propName.attribute("type") == "QString");
    EXPECT_TRUE(propName.text() == "LD_LIBRARY_PATH");

    auto propValue = propName.nextSiblingElement();
    EXPECT_TRUE(propValue.tagName() == "property");
    EXPECT_TRUE(propValue.attribute("name") == "value");
    EXPECT_TRUE(propValue.attribute("type") == "QString");
    EXPECT_TRUE(propValue.text() == "/usr/lib");
}

TEST_F(TestGtStructProperty, mementoDiffElementRemoved)
{
    // instead of removing an element, we simply switch before/after
    // as removing is not yet implemented

    TestObject obj;
    obj.addEnvironmentVar("PATH", "/usr/bin");
    auto beforeMemento = obj.toMemento();

    obj.addEnvironmentVar("LD_LIBRARY_PATH", "/usr/lib");
    auto afterMemento = obj.toMemento();

    // Yes, this is intentional
    GtObjectMementoDiff diff(afterMemento, beforeMemento);

    ASSERT_FALSE(diff.isNull());

    // check diff content
    auto objectElem = diff.documentElement();
    EXPECT_TRUE(objectElem.attribute("uuid") == obj.uuid());

    auto diffElem = objectElem.firstChildElement();
    ASSERT_TRUE(diffElem.tagName() == "diff-property-container-entry-remove");
    EXPECT_TRUE(diffElem.attribute("name") == "environmentVars");
    EXPECT_TRUE(diffElem.attribute("index") == "1");

    // next is the new entry
    EXPECT_EQ(1, diffElem.childNodes().size());
    auto newEntry = diffElem.firstChildElement();

    EXPECT_TRUE(newEntry.tagName() == "property");
    EXPECT_TRUE(newEntry.attribute("name") == obj.environmentVars.at(1).ident());
    EXPECT_TRUE(newEntry.attribute("type") == "EnvironmentVarsStruct");

    // it should have two sub properties
    ASSERT_EQ(2, newEntry.childNodes().size());

    auto propName = newEntry.firstChildElement();
    EXPECT_TRUE(propName.tagName() == "property");
    EXPECT_TRUE(propName.attribute("name") == "name");
    EXPECT_TRUE(propName.attribute("type") == "QString");
    EXPECT_TRUE(propName.text() == "LD_LIBRARY_PATH");

    auto propValue = propName.nextSiblingElement();
    EXPECT_TRUE(propValue.tagName() == "property");
    EXPECT_TRUE(propValue.attribute("name") == "value");
    EXPECT_TRUE(propValue.attribute("type") == "QString");
    EXPECT_TRUE(propValue.text() == "/usr/lib");
}

TEST_F(TestGtStructProperty, createByFactory)
{
    TestObject obj;

    auto const * prop = GtPropertyFactory::instance()->newProperty("EnvironmentVarsStruct", "id1", "id1");

    EXPECT_TRUE(prop == nullptr);

    auto func = [&obj](const QString&, const QString& ){
        return &obj.environmentVars.newEntry("EnvironmentVarsStruct");
    };

    GtPropertyFactory::instance()->registerProperty("EnvironmentVarsStruct", func);

    prop = GtPropertyFactory::instance()->newProperty("EnvironmentVarsStruct", "id1", "id1");

    ASSERT_TRUE(prop != nullptr);
    EXPECT_TRUE(prop->findProperty("value") != nullptr);
    EXPECT_TRUE(prop->findProperty("name") != nullptr);

    GtPropertyFactory::instance()->unregisterProperty("EnvironmentVarsStruct");
}
