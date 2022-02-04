#ifndef GTDATAZONETABLE_H
#define GTDATAZONETABLE_H

#include "gt_datamodel_exports.h"

#include "gt_object.h"
#include <QStringList>
#include <QVector>
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

    GtDataZoneTable(const QStringList& x,
                    const QStringList& y,
                    const QStringList& z,
                    QList<GtAbstractDataZone*> data);

    /**
     * @brief dataZone
     * @param x
     * @param y
     * @param z
     * @return
     */
    GtAbstractDataZone* dataZone(int x, int y, int z) const;

//    GtAbstractDataZoneList* dataZoneList(int x, int y, int z);

    /**
     * @brief dataZone
     * @param idx
     * @return
     */
    GtAbstractDataZone* dataZone(int idx) const;

    /**
     * @brief isValid
     * @return
     */
    bool isValid() const;

    /**
     * @brief isEmpty
     * @return
     */
    bool isEmpty() const;

    /**
     * @brief xAxis
     * @return
     */
    QStringList xAxis() const;

//    QStringList xAxisLists();

    /**
     * @brief xAxis
     * @param i
     * @return
     */
    QString xAxis(int i) const;

    /**
     * @brief yAxis
     * @return
     */
    QStringList yAxis() const;

    /**
     * @brief yAxis
     * @param i
     * @return
     */
    QString yAxis(int i) const;

    /**
     * @brief zAxis
     * @return
     */
    QStringList zAxis() const;

    /**
     * @brief zAxis
     * @param i
     * @return
     */
    QString zAxis(int i) const;

    /**
     * @brief params
     * @return
     */
    QStringList params() const;

    /**
     * @brief nXDims
     * @return
     */
    int nXDims() const;

    /**
     * @brief nYDims
     * @return
     */
    int nYDims() const;

    /**
     * @brief nZDims
     * @return
     */
    int nZDims() const;

    /**
     * @brief is0D
     * @return
     */
    bool is0D() const;

    /**
     * @brief nSubDims
     * @return
     */
    int nSubDims() const;

    /**
     * @brief allAxisTicks
     * @return
     */
    QVector<QVector<double> > allAxisTicks() const;

    /**
     * @brief allAxisTicksMap
     * @return
     */
    QMap<QString, QVector<double> > allAxisTicksMap() const;

    /**
     * @brief allAxisTicksString
     * @return
     */
    QList<QStringList> allAxisTicksString() const;

    /**
     * @brief allAxisTicksStringMap
     * @return
     */
    QMap<QString, QStringList> allAxisTicksStringMap() const;

    /**
     * @brief nMainDims
     * @return
     */
    int nMainDims() const;

    /**
     * @brief subAxisNames
     * @return
     */
    QStringList subAxisNames() const;

    /**
     * @brief axisTicks
     * @param id
     * @param dvecOut
     */
    void subAxisTicks(const QString& id, QVector<double>& dvecOut) const;

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
    double value2D(const QString& param, int mainX, int mainY, int mainZ,
                   double x, double y, bool* ok = nullptr) const;

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
    double value2D(const QString& param, const QString& mainX,
                   const QString& mainY, const QString& mainZ,
                   double x, double y, bool* ok = nullptr) const;

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
    double value1D(const QString& param, int mainX, int mainY, int mainZ,
                   double x, bool* ok = nullptr) const;

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
    double value1D(const QString& param, const QString& mainX,
                   const QString& mainY, const QString& mainZ,
                   double x, bool* ok = nullptr) const;

    /**
     * @brief value0D
     * @param param
     * @param mainX
     * @param mainY
     * @param mainZ
     * @param ok
     * @return
     */
    double value0D(const QString& param, int mainX, int mainY, int mainZ,
                   bool* ok = nullptr) const;

    double valueFrom0Ddata(const QString& param, GtAbstractDataZone* adz,
                           bool* ok = nullptr) const;

    /**
     * @brief value0D
     * @param param
     * @param mainX
     * @param mainY
     * @param mainZ
     * @param ok
     * @return
     */
    double value0D(const QString& param, const QString& mainX,
                   const QString& mainY, const QString& mainZ,
                   bool* ok = nullptr) const;

    double value0DfromOP(const QString& param, const QString& OP,
                         bool* ok = nullptr) const;

    /**
     * @brief axisTicks
     * @param id
     * @return
     */
    QStringList subAxisTicks(const QString& id) const;

    /**
     * @brief xAxisName
     * @return
     */
    QString xAxisName() const;

    /**
     * @brief yAxisName
     * @return
     */
    QString yAxisName() const;

    /**
     * @brief zAxisName
     * @return
     */
    QString zAxisName() const;

    /**
     * @brief xAxisIndexFromString
     * @param str
     * @return
     */
    int xAxisIndexFromString(const QString& str) const;

    /**
     * @brief yAxisIndexFromString
     * @param str
     * @return
     */
    int yAxisIndexFromString(const QString& str) const;

    /**
     * @brief zAxisIndexFromString
     * @param str
     * @return
     */
    int zAxisIndexFromString(const QString& str) const;

    /**
     * @brief unitFromParam
     * @return
     */
    QString unitFromParam(const QString& param) const;

    int dataSize() const;

    GtDataZoneTableMainAxis* xPtr() const;

    GtDataZoneTableMainAxis* yPtr() const;

    GtDataZoneTableMainAxis* zPtr() const;

    void setXaxisTitle(QString title);

    void setYaxisTitle(QString title);

    void setZaxisTitle(QString title);

    void setAxisActive(GtDataZoneTableMainAxis::AxisType axType, bool mod);

    bool onlyXaxisActive();

private:

    QList<GtAbstractDataZone *> data() const;

//    QList<GtAbstractDataZoneList *> dataList();

//    void onObjectDataMerged();

    /// DataZones
//    QList<GtAbstractDataZone*> m_data;

    GtDataZoneTableMainAxis* findAxis(GtDataZoneTableMainAxis::AxisType axType);
    void initAxes();
    void setTicks(const QStringList& z,
                  const QStringList& x,
                  const QStringList& y);
//    void processAbstractList(QList<GtAbstractDataZoneList*> data);
};

#endif // GTDATAZONETABLE_H
