#ifndef GTDATAZONETABLE_H
#define GTDATAZONETABLE_H

#include "gt_datamodel_exports.h"

#include "gt_object.h"
#include "QStringList"
#include "QVector"
#include "gt_datazonetablemainaxis.h"

class GtAbstractDataZone;
class GtAbstractDataZoneList;
class GtDataZone0D;

/**
 * @brief The GtDataZoneTable class
 */
class GT_DATAMODEL_EXPORT GtDataZoneTable : public GtObject
{
    Q_OBJECT

public:

    /**
     * @brief GtDataZoneTable
     */
    Q_INVOKABLE GtDataZoneTable();

    /**
     * @brief GtDataZoneTable
     * @param x
     * @param y
     * @param z
     * @param data
     */
//    GtDataZoneTable(QStringList x, QStringList y, QStringList z,
//                    QList<GtAbstractDataZoneList*> data);

    GtDataZoneTable(QStringList x, QStringList y, QStringList z,
                    QList<GtAbstractDataZone*> data);

    /**
     * @brief dataZone
     * @param x
     * @param y
     * @param z
     * @return
     */
    GtAbstractDataZone* dataZone(int x, int y, int z);

//    GtAbstractDataZoneList* dataZoneList(int x, int y, int z);

    /**
     * @brief dataZone
     * @param idx
     * @return
     */
    GtAbstractDataZone* dataZone(int idx);

    /**
     * @brief isValid
     * @return
     */
    bool isValid();

    /**
     * @brief isEmpty
     * @return
     */
    bool isEmpty();

    /**
     * @brief xAxis
     * @return
     */
    QStringList xAxis();

//    QStringList xAxisLists();

    /**
     * @brief xAxis
     * @param i
     * @return
     */
    QString xAxis(int i);

    /**
     * @brief yAxis
     * @return
     */
    QStringList yAxis();

    /**
     * @brief yAxis
     * @param i
     * @return
     */
    QString yAxis(int i);

    /**
     * @brief zAxis
     * @return
     */
    QStringList zAxis();

    /**
     * @brief zAxis
     * @param i
     * @return
     */
    QString zAxis(int i);

    /**
     * @brief params
     * @return
     */
    QStringList params();

    /**
     * @brief nXDims
     * @return
     */
    int nXDims();

    /**
     * @brief nYDims
     * @return
     */
    int nYDims();

    /**
     * @brief nZDims
     * @return
     */
    int nZDims();

    /**
     * @brief is0D
     * @return
     */
    bool is0D();

    /**
     * @brief nSubDims
     * @return
     */
    int nSubDims();

    /**
     * @brief allAxisTicks
     * @return
     */
    QVector<QVector<double> > allAxisTicks();

    /**
     * @brief allAxisTicksMap
     * @return
     */
    QMap<QString, QVector<double> > allAxisTicksMap();

    /**
     * @brief allAxisTicksString
     * @return
     */
    QList<QStringList> allAxisTicksString();

    /**
     * @brief allAxisTicksStringMap
     * @return
     */
    QMap<QString, QStringList> allAxisTicksStringMap();

    /**
     * @brief nMainDims
     * @return
     */
    int nMainDims();

    /**
     * @brief subAxisNames
     * @return
     */
    QStringList subAxisNames();

    /**
     * @brief axisTicks
     * @param id
     * @param dvecOut
     */
    void subAxisTicks(QString id, QVector<double>& dvecOut);

    /**
     * @brief value2D
     * @param param
     * @param mainX
     * @param mainY
     * @param mainZ
     * @param x
     * @param y
     * @param ok
     * @return
     */
    double value2D(QString param, int mainX, int mainY, int mainZ,
                   double x, double y, bool* ok = Q_NULLPTR);

    /**
     * @brief value2D
     * @param param
     * @param mainX
     * @param mainY
     * @param mainZ
     * @param x
     * @param y
     * @param ok
     * @return
     */
    double value2D(QString param, QString mainX, QString mainY, QString mainZ,
                   double x, double y, bool* ok = Q_NULLPTR);

    /**
     * @brief value1D
     * @param param
     * @param mainX
     * @param mainY
     * @param mainZ
     * @param x
     * @param ok
     * @return
     */
    double value1D(QString param, int mainX, int mainY, int mainZ,
                   double x, bool* ok = Q_NULLPTR);

    /**
     * @brief value1D
     * @param param
     * @param mainX
     * @param mainY
     * @param mainZ
     * @param x
     * @param ok
     * @return
     */
    double value1D(QString param, QString mainX, QString mainY, QString mainZ,
                   double x, bool* ok = Q_NULLPTR);

    /**
     * @brief value0D
     * @param param
     * @param mainX
     * @param mainY
     * @param mainZ
     * @param ok
     * @return
     */
    double value0D(QString param, int mainX, int mainY, int mainZ,
                   bool* ok = Q_NULLPTR);

    double valueFrom0Ddata(QString param, GtAbstractDataZone* adz,
                           bool* ok = Q_NULLPTR);

    /**
     * @brief value0D
     * @param param
     * @param mainX
     * @param mainY
     * @param mainZ
     * @param ok
     * @return
     */
    double value0D(QString param, QString mainX, QString mainY, QString mainZ,
                   bool* ok = Q_NULLPTR);

    double value0DfromOP(QString param, QString OP,
                   bool* ok = Q_NULLPTR);

    /**
     * @brief axisTicks
     * @param id
     * @return
     */
    QStringList subAxisTicks(QString id);

    /**
     * @brief xAxisName
     * @return
     */
    QString xAxisName();

    /**
     * @brief yAxisName
     * @return
     */
    QString yAxisName();

    /**
     * @brief zAxisName
     * @return
     */
    QString zAxisName();

    /**
     * @brief xAxisIndexFromString
     * @param str
     * @return
     */
    int xAxisIndexFromString(QString str);

    /**
     * @brief yAxisIndexFromString
     * @param str
     * @return
     */
    int yAxisIndexFromString(QString str);

    /**
     * @brief zAxisIndexFromString
     * @param str
     * @return
     */
    int zAxisIndexFromString(QString str);

    /**
     * @brief unitFromParam
     * @return
     */
    QString unitFromParam(QString param);

    int dataSize();

    GtDataZoneTableMainAxis* xPtr();

    GtDataZoneTableMainAxis* yPtr();

    GtDataZoneTableMainAxis* zPtr();

    void setXaxisTitle(QString title);

    void setYaxisTitle(QString title);

    void setZaxisTitle(QString title);

    void setAxisActive(GtDataZoneTableMainAxis::AxisType axType, bool mod);

    bool onlyXaxisActive();

private:

    QList<GtAbstractDataZone *> data();

//    QList<GtAbstractDataZoneList *> dataList();

//    void onObjectDataMerged();

    /// DataZones
//    QList<GtAbstractDataZone*> m_data;

    GtDataZoneTableMainAxis* findAxis(GtDataZoneTableMainAxis::AxisType axType);
    void initAxes();
    void setTicks(QStringList z, QStringList x, QStringList y);
//    void processAbstractList(QList<GtAbstractDataZoneList*> data);
};

#endif // GTDATAZONETABLE_H
