#include "gt_datazonetable.h"
#include "gt_abstractdatazone.h"
#include "QStringList"
#include "QVector"
#include "QsLog.h"
#include "gt_datazone.h"
#include "gt_datazonetablemainaxis.h"
#include "gt_datazone0d.h"
#include "gt_externalizedobjecthelper.h"
//#include "gt_objectfactory.h"



GtDataZoneTable::GtDataZoneTable()
{
    initAxes();
}

//GtDataZoneTable::GtDataZoneTable(QStringList x, QStringList y, QStringList z,
//                                 QList<GtAbstractDataZoneList*> data)
//{
//    initAxes();

//    setTicks(z, x, y);

//    processAbstractList(data);
//}

GtDataZoneTable::GtDataZoneTable(const QStringList& x,
                                 const QStringList& y,
                                 const QStringList& z,
                                 QList<GtAbstractDataZone *> data)
{
    initAxes();

    setTicks(z, x, y);

    // make sure that for each given abstractDataZone an abstractDataZoneList
    // is created to be consequent with new structure
    //      (Table->DataZoneList->DataZone)
//    QList<GtAbstractDataZoneList *> retval;

    for (GtAbstractDataZone* adz : data)
    {
        if (adz != Q_NULLPTR)
        {
            appendChild(adz);
        }
    }

//    processAbstractList(retval);
}

void
GtDataZoneTable::initAxes()
{
    GtDataZoneTableMainAxis* x =
            new GtDataZoneTableMainAxis("mainX", GtDataZoneTableMainAxis::X);
    x->setDefault(true);
    appendChild(x);

    GtDataZoneTableMainAxis* y =
            new GtDataZoneTableMainAxis("mainY", GtDataZoneTableMainAxis::Y);
    y->setDefault(true);
    appendChild(y);

    GtDataZoneTableMainAxis* z =
            new GtDataZoneTableMainAxis("mainZ", GtDataZoneTableMainAxis::Z);
    z->setDefault(true);
    appendChild(z);
}

void
GtDataZoneTable::setTicks(const QStringList& z,
                          const QStringList& x,
                          const QStringList& y)
{
    xPtr()->setTicks(x);
    yPtr()->setTicks(y);
    zPtr()->setTicks(z);
}

//void
//GtDataZoneTable::processAbstractList(QList<GtAbstractDataZoneList*> data)
//{
//    for (GtAbstractDataZoneList* adz : data)
//    {
//        if (adz != Q_NULLPTR)
//        {
//            appendChild(adz);
//        }
//    }

//    if(!isValid())
//    {
//        gtWarning() << tr("Data Zone Table is not valid!");
//    }

//    if(isEmpty())
//    {
//        gtWarning() << tr("Data Zone Table is empty!");
//    }
//}

GtDataZoneTableMainAxis*
GtDataZoneTable::xPtr() const
{
    if (findDirectChild<GtDataZoneTableMainAxis*>("mainX") == Q_NULLPTR)
    {
        gtError() << "xAxis is Null";

        return Q_NULLPTR;
    }

    return findDirectChild<GtDataZoneTableMainAxis*>("mainX");
}

GtDataZoneTableMainAxis*
GtDataZoneTable::yPtr() const
{
    return findDirectChild<GtDataZoneTableMainAxis*>("mainY");
}

GtDataZoneTableMainAxis*
GtDataZoneTable::zPtr() const
{
    return findDirectChild<GtDataZoneTableMainAxis*>("mainZ");
}

void
GtDataZoneTable::setXaxisTitle(QString title)
{
    xPtr()->setTitle(title);
}

void
GtDataZoneTable::setYaxisTitle(QString title)
{
    yPtr()->setTitle(title);
}

void
GtDataZoneTable::setZaxisTitle(QString title)
{
    zPtr()->setTitle(title);
}

void
GtDataZoneTable::setAxisActive(GtDataZoneTableMainAxis::AxisType axType,
                               bool mod)
{
    GtDataZoneTableMainAxis* axis = findAxis(axType);
    {
        if (axis == nullptr)
        {
            // error
            return;
        }

        axis->setActive(mod);
    }
}

bool
GtDataZoneTable::onlyXaxisActive()
{
    if (xPtr() != nullptr && yPtr() != nullptr && zPtr() != nullptr)
    {
        if (xPtr()->active() && !yPtr()->active() && !zPtr()->active())
        {
            return true;
        }
    }

    return false;
}

QList<GtAbstractDataZone*>
GtDataZoneTable::data() const
{
    return findDirectChildren<GtAbstractDataZone*>();
}

//QList<GtAbstractDataZoneList *>
//GtDataZoneTable::dataList()
//{
//    return findDirectChildren<GtAbstractDataZoneList*>();
//}

GtAbstractDataZone*
GtDataZoneTable::dataZone(int x, int y, int z) const
{
    // x, y, z indices of their axes, starting at 0
    int idx = xPtr()->size() * yPtr()->size() * z + xPtr()->size() * y + x;

    if (idx > data().size())
    {
        // this should not happen
        return nullptr;
    }

    return data().at(idx);
}

//GtAbstractDataZoneList*
//GtDataZoneTable::dataZoneList(int x, int y, int z)
//{
//    // x, y, z indices of their axes, starting at 0
//    int idx = xPtr()->size() * yPtr()->size() * z + xPtr()->size() * y + x;

//    if (idx > dataList().size())
//    {
//        // this should not happen
//        return Q_NULLPTR;
//    }

//    return dataList().at(idx);
//}

GtAbstractDataZone*
GtDataZoneTable::dataZone(int idx) const
{
    if (idx > data().size())
    {
        // this should not happen
        return nullptr;
    }

    return data().at(idx);
}

bool
GtDataZoneTable::isValid() const
{
    if (xPtr()->size() * yPtr()->size() * zPtr()->size() != data().size())
    {
        gtError() << "GtDataZoneTable::isValid() data size not corrent";
        return false;
    }

    for (GtAbstractDataZone* dz : data())
    {
        if (dz == nullptr)
        {
            gtError() << "GtDataZoneTable::isValid() data is null";
            return false;
        }
    }

    return true;
}

bool
GtDataZoneTable::isEmpty() const
{
    return data().isEmpty();
}

QStringList
GtDataZoneTable::xAxis() const
{
    return xPtr()->ticks();
}

QString
GtDataZoneTable::xAxis(int i) const
{
    if (i < xPtr()->size() || i < 0)
    {
        return xPtr()->ticks().at(i);
    }

    return "X entry Not found";
}

QStringList
GtDataZoneTable::yAxis() const
{
    return yPtr()->ticks();
}

QString
GtDataZoneTable::yAxis(int i) const
{
    if (i < yPtr()->size() || i < 0)
    {
        return yPtr()->ticks().at(i);
    }

    return "Y entry Not found";
}

QStringList
GtDataZoneTable::zAxis() const
{
    return zPtr()->ticks();
}

QString
GtDataZoneTable::zAxis(int i) const
{
    if (i < zPtr()->size() || i < 0)
    {
        return zPtr()->ticks().at(i);
    }

    return "Z entry Not found";
}

int
GtDataZoneTable::nXDims() const
{
    return xPtr()->size();
}

int
GtDataZoneTable::nYDims() const
{
    return yPtr()->size();
}

int
GtDataZoneTable::nZDims() const
{
    return zPtr()->size();
}

bool
GtDataZoneTable::is0D() const
{
    if (data().at(0) != Q_NULLPTR)
    {
        return data().at(0)->is0D();
    }

    return false;
}

int
GtDataZoneTable::nSubDims() const
{
    if (data().size() == 0)
    {
        gtWarning() << tr("There is no GtAbstractDataZone in GtDataZoneTable '")
                  << objectName() << "'";
        return -1;
    }

    if (data().at(0) != Q_NULLPTR)
    {
        return data().at(0)->nDims();
    }

    return 0;
}

QVector<QVector<double> >
GtDataZoneTable::allAxisTicks() const
{
    QVector<QVector<double> > retval;

    if (nSubDims() > 0)
    {
        GtDataZone* dz = qobject_cast<GtDataZone*>(data().at(0));

        if (dz == Q_NULLPTR)
        {
            // bad error
            gtDebug() << tr("cannot cast datazonetable into datazone");
        }
        else
        {
            retval = dz->allAxisTicks();
        }
    }

    return retval;
}

QMap<QString, QVector<double> >
GtDataZoneTable::allAxisTicksMap() const
{
    QMap<QString, QVector<double>> retval;

    if (nSubDims() > 0)
    {
        GtDataZone* dz = qobject_cast<GtDataZone*>(data().at(0));

        if (dz == Q_NULLPTR)
        {
            // bad error
            gtDebug() << tr("cannot cast datazonetable into datazone");
        }
        else
        {
            retval = dz->allAxisTicksMap();
        }
    }

    return retval;
}

QList<QStringList>
GtDataZoneTable::allAxisTicksString() const
{
    QVector<QVector<double> > retval = allAxisTicks();

    QList<QStringList> retvalString;

    for (const QVector<double>& vec : retval)
    {
        QStringList l;

        for (const double& val : vec)
        {
            l.append(QString::number(val));
        }

        retvalString.append(l);
    }

    return retvalString;
}

QMap<QString, QStringList>
GtDataZoneTable::allAxisTicksStringMap() const
{
    QMap<QString, QVector<double>> ticksDoubleMap = allAxisTicksMap();

    QMap<QString, QStringList> retval;

    QMap<QString, QVector<double>>::iterator i;
    for (i = ticksDoubleMap.begin(); i != ticksDoubleMap.end(); i++)
    {
        QStringList l;

        for (const double& val : i.value())
        {
            l.append(QString::number(val));
        }

        retval.insert(i.key(), l);
    }

    return retval;
}

int
GtDataZoneTable::nMainDims() const
{
    int dims = 0;
    if (nXDims() > 1)
        dims++;

    if (nYDims() > 1)
        dims++;

    if (nZDims() > 1)
        dims++;

    return dims;
}

QStringList
GtDataZoneTable::subAxisNames() const
{
    QStringList retval;

    if (nSubDims() > 0)
    {
        GtDataZone* dz = qobject_cast<GtDataZone*>(data().at(0));

        if (dz == Q_NULLPTR)
        {
            // bad error
            gtDebug() << tr("cannot cast datazonetable into datazone");
        }
        else
        {
            retval = dz->axisNames();
        }
    }

    return retval;
}

void
GtDataZoneTable::subAxisTicks(const QString& id, QVector<double>& dvecOut) const
{
    GtDataZone* dz = Q_NULLPTR;

    dz = qobject_cast<GtDataZone*>(data().at(0));

    if (dz == Q_NULLPTR)
    {
        return;
    }

    dz->axisTicks(id, dvecOut);
}

double
GtDataZoneTable::value2D(const QString& param, int mainX, int mainY, int mainZ,
                         double x, double y, bool* ok) const
{
    GtExternalizedObjectHelper<GtAbstractDataZone> adz
            (dataZone(mainX, mainY, mainZ), GtExternalizedObject::Discard);

    if (!adz.isValid())
    {
        if (ok)
        {
            *ok = false;
        }
        return 0.0;
    }

    GtDataZone* dzND = Q_NULLPTR;
    dzND = qobject_cast<GtDataZone*>(adz);

    if (dzND == Q_NULLPTR)
    {
        if (ok)
        {
            *ok = false;
        }
        return 0.0;
    }

    return dzND->value2D(param, x, y, ok);
}

double
GtDataZoneTable::value2D(const QString& param,
                         const QString& mainX, const QString& mainY,
                         const QString& mainZ,
                         double x, double y, bool* ok) const
{
    int mainXnumber = xPtr()->indexOf(mainX);
    int mainYnumber = yPtr()->indexOf(mainY);
    int mainZnumber = zPtr()->indexOf(mainZ);

    return value2D(param, mainXnumber, mainYnumber, mainZnumber,
                   x, y, ok);
}

double
GtDataZoneTable::value1D(const QString& param, int mainX, int mainY, int mainZ,
                         double x, bool* ok) const
{
    GtExternalizedObjectHelper<GtAbstractDataZone> adz
            (dataZone(mainX, mainY, mainZ), GtExternalizedObject::Discard);

    if (!adz.isValid())
    {
        if (ok)
        {
            *ok = false;
        }
        return 0.0;
    }

    GtDataZone* dzND = Q_NULLPTR;
    dzND = qobject_cast<GtDataZone*>(adz);

    if (dzND == Q_NULLPTR)
    {
        if (ok)
        {
            *ok = false;
        }
        return 0.0;
    }

    return dzND->value1D(param, x, ok);
}

double
GtDataZoneTable::value1D(const QString& param,
                         const QString& mainX,
                         const QString& mainY,
                         const QString& mainZ,
                         double x, bool* ok) const
{
    int mainXnumber = xPtr()->indexOf(mainX);
    int mainYnumber = yPtr()->indexOf(mainY);
    int mainZnumber = zPtr()->indexOf(mainZ);

    return value1D(param, mainXnumber, mainYnumber, mainZnumber,
                   x, ok);
}

double
GtDataZoneTable::value0D(const QString& param, int mainX, int mainY, int mainZ,
                         bool* ok) const
{
    GtExternalizedObjectHelper<GtAbstractDataZone> adz
            (dataZone(mainX, mainY, mainZ), GtExternalizedObject::Discard);

    if (!adz.isValid())
    {
        if (ok)
        {
            *ok = false;
        }

        return 0.0;
    }

    return valueFrom0Ddata(param, adz, ok);
}

double
GtDataZoneTable::valueFrom0Ddata(const QString& param, GtAbstractDataZone* adz,
                                 bool* ok) const
{
    GtExternalizedObjectHelper<GtDataZone0D> dz0D
            (adz, GtExternalizedObject::Discard);

    if (!dz0D.isValid())
    {
        if (ok != Q_NULLPTR)
        {
            *ok = false;
        }

        return 0.0;
    }

    return dz0D->value(param, ok);
}

double
GtDataZoneTable::value0D(const QString& param,
                         const QString& mainX, const QString& mainY,
                         const QString& mainZ,
                         bool* ok) const
{
    int mainXnumber = xPtr()->indexOf(mainX, ok);

    if (ok)
    {
        if (*ok == false)
        {
            return 0.0;
        }
    }

    int mainYnumber = yPtr()->indexOf(mainY, ok);

    if (ok)
    {
        if (*ok == false)
        {
            return 0.0;
        }
    }

    int mainZnumber = zPtr()->indexOf(mainZ, ok);

    if (ok)
    {
        if (*ok == false)
        {
            return 0.0;
        }
    }

    return value0D(param, mainXnumber, mainYnumber, mainZnumber, ok);
}

double
GtDataZoneTable::value0DfromOP(QString param, QString OP, bool *ok) const
{
    int mainXnumber = xPtr()->indexOf(OP);

    GtExternalizedObjectHelper<GtAbstractDataZone> adz
            (dataZone(mainXnumber, 0, 0), GtExternalizedObject::Discard);

    if (!adz.isValid())
    {
        if (ok != Q_NULLPTR)
        {
            *ok = false;
        }

        return 0.0;
    }

    return valueFrom0Ddata(param, adz, ok);
}

QStringList
GtDataZoneTable::subAxisTicks(const QString& id) const
{
    QStringList retval;

    GtExternalizedObjectHelper<GtDataZone> dz
            (data().at(0), GtExternalizedObject::Discard);

    if (!dz.isValid())
    {
        // e.g. if it is 0D-data
        return retval;
    }

    return dz->axisTickLabels(id);
}

QString
GtDataZoneTable::xAxisName() const
{
    return xPtr()->name();
}

QString
GtDataZoneTable::yAxisName() const
{
    return yPtr()->name();
}

QString
GtDataZoneTable::zAxisName() const
{
    return zPtr()->name();
}

int
GtDataZoneTable::xAxisIndexFromString(const QString& str) const
{
    return xPtr()->indexOf(str);
}

int
GtDataZoneTable::yAxisIndexFromString(const QString& str) const
{
    return yPtr()->indexOf(str);
}

int
GtDataZoneTable::zAxisIndexFromString(const QString& str) const
{
    return zPtr()->indexOf(str);
}

QStringList
GtDataZoneTable::params() const
{
    GtExternalizedObjectHelper<GtAbstractDataZone> dz
            (data().first(), GtExternalizedObject::Discard);

    return dz->params();
}

QString
GtDataZoneTable::unitFromParam(const QString& param) const
{
    QString retval;

    GtExternalizedObjectHelper<GtAbstractDataZone> dz
            (data().first(), GtExternalizedObject::Discard);

    retval = dz->unit(param);

    return retval;
}

int
GtDataZoneTable::dataSize() const
{
    return data().size();
}


//void
//GtDataZoneTable::onObjectDataMerged()
//{
//    m_data.append(findDirectChildren<GtAbstractDataZone*>());
//}

GtDataZoneTableMainAxis*
GtDataZoneTable::findAxis(GtDataZoneTableMainAxis::AxisType axType)
{
    GtDataZoneTableMainAxis* retval = Q_NULLPTR;

    QList<GtDataZoneTableMainAxis*> axes =
            findDirectChildren<GtDataZoneTableMainAxis*>();

    for (GtDataZoneTableMainAxis* axis : axes)
    {
        if (axis == Q_NULLPTR)
        {
            // error
            continue;
        }

        if (axis->axisType() == axType)
        {
            retval = axis;
        }
    }

    return retval;
}
