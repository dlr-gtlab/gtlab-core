#include <QDebug>

#include "gtest/gtest.h"
#include "gt_datazonetable.h"
#include "gt_datazone0d.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_testhelper.h"
#include "gt_externalizationsettings.h"

/// This is a test fixture that does a init for each test
class TestGtDataZoneTable : public ::testing::Test
{
protected:
    virtual void SetUp()
    {

        QStringList x;
        x.append("OP1");
        x.append("OP2");
        x.append("OP3");
        x.append("OP4");
        QStringList y;
        y.append("Stage1");
        QStringList z;
        z.append("Station1");

        QList<GtAbstractDataZone*> data;
        GtAbstractDataZone* datazone1 = new GtDataZone0D();
        GtAbstractDataZone* datazone2 = new GtDataZone0D();
        GtAbstractDataZone* datazone3 = new GtDataZone0D();
        GtAbstractDataZone* datazone4 = new GtDataZone0D();
        data.append(datazone1);
        data.append(datazone2);
        data.append(datazone3);
        data.append(datazone4);

        obj1 = new GtDataZoneTable(x, y, z, data);
        obj1->setFactory(gtObjectFactory);

        qDebug() << obj1->toMemento().toByteArray();

    }

    virtual void TearDown()
    {
    }

    GtDataZoneTable* obj1;
    GtDataZoneTable obj2;
};

TEST_F(TestGtDataZoneTable, copy)
{
    ASSERT_FALSE(obj1 == nullptr);

    int subDims = obj1->nSubDims();

    ASSERT_TRUE(subDims == 0);

    GtDataZoneTable* dztCopy =
            qobject_cast<GtDataZoneTable*>(obj1->copy());

    ASSERT_FALSE(dztCopy == nullptr);

    int subDimsCopy = dztCopy->nSubDims();

    ASSERT_TRUE(subDimsCopy == 0);
}

TEST_F(TestGtDataZoneTable, isValid)
{
    QStringList x;
    x.append("OP1");
    x.append("OP2");
    x.append("OP3");
    x.append("OP4");
    QStringList y;
    y.append("Stage1");
    QStringList z;
    z.append("Station1");

    QList<GtAbstractDataZone*> data;
    GtAbstractDataZone* datazone1 = new GtDataZone0D();
    GtAbstractDataZone* datazone2 = new GtDataZone0D();
    GtAbstractDataZone* datazone3 = new GtDataZone0D();
    GtAbstractDataZone* datazone4 = new GtDataZone0D();
    data.append(datazone1);
    data.append(datazone2);
    data.append(datazone3);
    data.append(datazone4);

    GtDataZoneTable obj1(x, y, z, data);

    // test isValid
    ASSERT_TRUE(obj1.isValid());

    QList<GtAbstractDataZone*> dataNew;
    QStringList xNew;
    QStringList yNew;
    QStringList zNew;

    // other constructur invalid
    xNew.append("OP1");
    xNew.append("OP2");
    xNew.append("OP3");
    xNew.append("OP4");
    yNew.append("Stage1");
    zNew.append("Station1");

    dataNew.append(datazone1);
    dataNew.append(datazone2);
    dataNew.append(datazone3);

    GtDataZoneTable obj3(xNew, yNew, zNew, dataNew);



//    int num = obj1.xPtr()->size() * obj1.yPtr()->size() * obj1.zPtr()->size();
//    int numsecond = obj1.dataSize();

//    qDebug() << "num = " << num << ", numsecond = " << numsecond;

    //ASSERT_TRUE(obj2.isValid());
    ASSERT_FALSE(obj3.isValid());
}


TEST_F(TestGtDataZoneTable, isEmpty)
{
    QStringList x;
    x.append("OP1");
    x.append("OP2");
    x.append("OP3");
    x.append("OP4");
    QStringList y;
    y.append("Stage1");
    QStringList z;
    z.append("Station1");

    QList<GtAbstractDataZone*> data;
    GtAbstractDataZone* datazone1 = new GtDataZone0D();
    GtAbstractDataZone* datazone2 = new GtDataZone0D();
    GtAbstractDataZone* datazone3 = new GtDataZone0D();
    GtAbstractDataZone* datazone4 = new GtDataZone0D();
    data.append(datazone1);
    data.append(datazone2);
    data.append(datazone3);
    data.append(datazone4);

    GtDataZoneTable obj1(x, y, z, data);
    GtDataZoneTable obj2;

    ASSERT_FALSE(obj1.isEmpty());
    ASSERT_TRUE(obj2.isEmpty());
}

TEST_F(TestGtDataZoneTable, fetch0D)
{
    // enable externalization
    gtExternalizationSettings->onEnbaleExternalizationChanged(true);

    // create datazones
    GtDataZone0D* datazone1 = new GtDataZone0D();
    GtDataZone0D* datazone2 = new GtDataZone0D();
    GtDataZone0D* datazone3 = new GtDataZone0D();
    GtDataZone0D* datazone4 = new GtDataZone0D();

    QList<GtAbstractDataZone*> data;
    data.append(datazone1);
    data.append(datazone2);
    data.append(datazone3);
    data.append(datazone4);

    QStringList params(gtTestHelper->randomStringList(42));
    QStringList units(QVector<QString>(42, "[-]").toList());

    // set random data and externalize each datazone
    for (GtAbstractDataZone* adz : data)
    {
        GtDataZone0D* obj = qobject_cast<GtDataZone0D*>(adz);
        ASSERT_NE(obj, nullptr);

        obj->setData(params, gtTestHelper->randomDataVector(42), units);

        ASSERT_TRUE(obj->releaseData(GtExternalizedObject::Externalize));
    }

    // create datazone table
    QStringList x = { "OP1", "OP2", "OP3", "OP4" };
    QStringList y = { "Stage1" };
    QStringList z = { "Station1" };
    GtDataZoneTable dzt(x, y, z, data);

    EXPECT_FALSE(dzt.isEmpty());
    EXPECT_TRUE(dzt.isValid());
    EXPECT_TRUE(dzt.is0D());

    // all datazones should be externalized
    EXPECT_FALSE(dzt.dataZone(0)->isFetched());
    EXPECT_FALSE(dzt.dataZone(1)->isFetched());
    ASSERT_FALSE(dzt.dataZone(2)->isFetched());
    ASSERT_FALSE(dzt.dataZone(3)->isFetched());

    // functions should auto fetch the datazones if necessary
    EXPECT_EQ(dzt.params(), params);
    EXPECT_EQ(dzt.unitFromParam(params.at(0)), "[-]");

    bool ok = true;
    double value = dzt.value0D(params.at(5), 0, 0, 0, &ok);
    EXPECT_TRUE(ok);

    // fetch the data and check value
    ASSERT_TRUE(datazone1->fetchData());
    EXPECT_EQ(value, datazone1->value(params.at(5)));
    ASSERT_TRUE(datazone1->releaseData(GtExternalizedObject::Externalize));
    ASSERT_FALSE(datazone1->isFetched());

    // disable externalization
    gtExternalizationSettings->onEnbaleExternalizationChanged(false);

    // all datazones should be externalized -> data should not be accessible
    EXPECT_EQ(dzt.params().length(), 0);
    EXPECT_EQ(dzt.unitFromParam(params.at(0)), QString());
}
