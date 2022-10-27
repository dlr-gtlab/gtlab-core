/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_dummyobject.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 04.05.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */
#include "gtest/gtest.h"

#include <QDir>

#include "gt_objectmemento.h"
#include "gt_objectmementodiff.h"
#include "gt_testhelper.h"
#include "gt_objectfactory.h"

#include "test_gt_object.h"

/// This is a test fixture that does a init for each test
class TestGtDummyObject : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_obj.setObjectName("dummy_object");
    }

    virtual void TearDown()
    {
    }

    TestSpecialGtObject m_obj;

};

TEST_F(TestGtDummyObject, toMemento)
{
    // make sure, the object is not known to the factory
    GtObjectFactory::instance()
            ->unregisterClass(TestSpecialGtObject::staticMetaObject);

    QDir tmpDir = gtTestHelper->newTempDir();
    tmpDir.cdUp();

    // create object memento of unknown object
    m_obj.setDouble(123.4);
    m_obj.doubleVec << 2.4 << 2.6 << 2.7;
    m_obj.boolList << true << false << true;

    GtObjectMemento memento = m_obj.toMemento();
    memento.calculateHashes();

    // write memento to temporary file
    QFile memFile(tmpDir.absoluteFilePath("memento.xml"));
    ASSERT_TRUE(memFile.open(QIODevice::WriteOnly | QIODevice::Text));
    QTextStream inputStream(&memFile);

    inputStream << "<? memento of object "
                << memento.fullHash().toHex() << " ?>\n";
    inputStream << memento.toByteArray();

    ASSERT_FALSE(memento.isNull());

    ASSERT_FALSE(gtObjectFactory->knownClass(
                     GT_CLASSNAME(TestSpecialGtObject)));

    GtObject* newObj = memento.restore(gtObjectFactory);

    ASSERT_FALSE(newObj == nullptr);

    ASSERT_TRUE(newObj->isDummy());

    GtObjectMemento dummyMemento = newObj->toMemento();
    dummyMemento.calculateHashes();

    // write memento to temporary file
    inputStream << "<? memento of dummy object "
                << dummyMemento.fullHash().toHex() << " ?>\n";

    inputStream << dummyMemento.toByteArray();

    ASSERT_FALSE(dummyMemento.isNull());

    GtObjectMementoDiff diff(memento, dummyMemento);

    GtObjectMemento dummyMemento2(dummyMemento.toByteArray());
    dummyMemento2.calculateHashes();

    // write memento to temporary file
    inputStream << "<? memento from dummy memento "
                << dummyMemento2.fullHash().toHex() << " ?>\n";

    inputStream << dummyMemento2.toByteArray();

    ASSERT_FALSE(dummyMemento2.isNull());

    GtObjectMementoDiff diff2(memento, dummyMemento2);
    GtObjectMementoDiff diff3(dummyMemento, dummyMemento2);

    // write memento to temporary file
    inputStream << "<? diff1 - obj1 - obj2 ?>\n";
    inputStream << diff.toByteArray();
    inputStream << "<? diff2 - obj1 - obj3 ?>\n";
    inputStream << diff2.toByteArray();
    inputStream << "<? diff3 - obj2 - obj3 ?>\n";
    inputStream << diff3.toByteArray();
    memFile.close();

    const QString type("double");
    std::string str = type.toStdString();
    const char* p = str.c_str();
    static QVariant::Type v_type = QVariant::nameToType(p);
    QVariant variant(QString("0"));

    qDebug() << "#### type name before = " << variant.typeName();
    qDebug() << "#### value before = " << variant;

    variant.convert(v_type);

    qDebug() << "#### type name after = " << variant.typeName();
    qDebug() << "#### value after = " << variant;

    ASSERT_TRUE(variant.type() == v_type);


    ASSERT_TRUE(diff.isNull());
    ASSERT_TRUE(diff2.isNull());
    ASSERT_TRUE(diff3.isNull());
}
