#include <QDebug>

#include "gtest/gtest.h"
#include "gt_datazonetable.h"
#include "gt_datazone0d.h"
#include "gt_objectfactory.h"
#include "gt_objectmemento.h"

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
    ASSERT_FALSE(obj1 == Q_NULLPTR);

    int subDims = obj1->nSubDims();

    ASSERT_TRUE(subDims == 0);

    GtDataZoneTable* dztCopy =
            qobject_cast<GtDataZoneTable*>(obj1->copy());

    ASSERT_FALSE(dztCopy == Q_NULLPTR);

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
