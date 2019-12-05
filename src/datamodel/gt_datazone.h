/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#ifndef GTDATAZONE_H
#define GTDATAZONE_H

#include "gt_datamodel_exports.h"

#include <QPointer>

#include "gt_abstractdatazone.h"
#include "gt_table.h"
#include "gt_stringproperty.h"

/**
 * @brief The GtDataZone class
 */
class GT_DATAMODEL_EXPORT GtDataZone : public GtAbstractDataZone
{
    Q_OBJECT

    Q_PROPERTY(QString description READ description WRITE setDescription)

public:
    /**
     * @brief GtDataZone
     */
    Q_INVOKABLE GtDataZone();

    /**
     * @brief returns value from 1-dimensional datazone
     * @param param parameter name
     * @param x0 axis tick
     * @param ok control flag
     * @return
     */
    double value1D(QString param, double x0, bool* ok = Q_NULLPTR);

    /**
     * @brief returns vector of values for 1D data zone based on the
     * defined axis ticks
     * @param param parameter name
     * @param ok control flag
     * @return
     */
    QVector<double> value1DVector(QString param, bool* ok = Q_NULLPTR);

    /**
     * @brief returns vector of values for 1D data zone based on the
     * defined axis ticks in the argument vector
     * @param param parameter name
     * @param ticks axis ticks
     * @param ok control flag
     * @return
     */
    QVector<double> value1DVector(QString param, QVector<double> ticks,
                                  bool* ok = Q_NULLPTR);

    /**
     * @brief returns value from 2-dimensional table
     * @param param parameter name
     * @param x0 first dimension axis tick
     * @param x1 second dimension axis tick
     * @param ok control flag
     * @return
     */
    double value2D(QString param, double x0, double x1, bool* ok = Q_NULLPTR);

    /**
     * @brief returns value from 3-dimensional table
     * @param param parameter name
     * @param x0 first dimension axis tick
     * @param x1 second dimension axis tick
     * @param x2 third dimension axis tick
     * @param ok control flag
     * @return
     */
    double value3D(QString param, double x0, double x1, double x2,
                   bool* ok = Q_NULLPTR);

    /**
     * @brief returns value from 4-dimensional table
     * @param param parameter name
     * @param x0 first dimension axis tick
     * @param x1 second dimension axis tick
     * @param x2 third dimension axis tick
     * @param x3 fourth dimensio axis tick
     * @param ok control flag
     * @return
     */
    double value4D(QString param, double x0, double x1, double x2, double x3,
                   bool* ok = Q_NULLPTR);

    /**
     * @brief set 1-dimensional data to table
     * @param params parameter names
     * @param ticks axis ticks
     * @param axisName1 axis name
     * @param vals parameter names and values
     * @param units parameter units
     */
    void setData1D(QStringList params,
                   QVector<double> ticks,
                   QString axisName1,
                   QMap<QString, QVector<double> > vals,
                   QStringList units);

    /**
     * @brief set 1-dimensional data to table
     * @param params parameter names
     * @param vals axis ticks and values
     * @param axisName1 axis name
     * @param units parameter units
     */
    void setData1D(QStringList params,
                   QMap<double, QVector<double> > vals,
                   QString axisName1,
                   QStringList units);

    /**
     * @brief set 2-dimensional data to table
     * @param params parameter names
     * @param ticks1 ticks of first axis
     * @param ticks2 ticks of second axis
     * @param axisName1 name of first axis
     * @param axisName2 name of second axis
     * @param vals parameter names and values
     * @param units parameter units
     */
    void setData2D(QStringList params,
              QVector<double> ticks1,
              QVector<double> ticks2,
              QString axisName1, QString axisName2,
              QMap<QString, QVector<double> > vals,
                   QStringList units);

    /**
     * @brief returns unit for a certain parameter
     * @param param parameter name
     * @return
     */
    virtual QString unit(QString param) Q_DECL_OVERRIDE;

    /**
     * @brief stores axis ticks in argument vector
     * @param id axis id
     * @param axTicks axis ticks vector
     */
    void axisTicks(QString id, QVector<double>& axTicks);

    /**
     * @brief returns axis ticks in argument string list
     * @param id axis id
     * @param axTicks axis ticks string list
     */
    void axisTicks(QString id, QStringList& axTicks);

    /**
     * @brief returns axis ticks as vector of type double
     * @param id axis id
     * @return axis ticks
     */
    QVector<double> axisTicks(QString id);

    /**
     * @brief returns axis ticks as list of type QString
     * @param id axis id
     * @return axis ticks
     */
   QStringList axisTickStrings(QString id);

    /**
     * @brief returns axis tick labels i.e. for plots
     * @param id axis id
     * @return axis labels
     */
    QStringList axisTickLabels(QString id);

    /**
     * @brief returns ticks of all table axis in one vector
     * @return all axis ticks
     */
    QVector< QVector<double> > allAxisTicks();

    /**
     * @brief returns map of axis names and approriate ticks in form of a vector
     * @return all axis names and ticks
     */
    QMap<QString, QVector<double> > allAxisTicksMap();

    /**
     * @brief returns all axis names
     * @return axis names
     */
    QStringList axisNames();

    /**
     * @brief check function if table is 0-dimensional
     * @return true in case that dimension of table is 0
     */
    virtual bool is0D() Q_DECL_OVERRIDE;

    /**
     * @brief returns the number of dimensions
     * @return number of dimensions
     */
    virtual int nDims() Q_DECL_OVERRIDE;

    /**
     * @brief addModuleName
     * @param moduleName
     */
    void addModuleName(QString suffix) Q_DECL_OVERRIDE;

    /**
     * @brief setDescription
     * @param description
     */
    void setDescription(const QString &description);

    /**
     * @brief returns data zone description
     * @return data zone description
     */
    QString description() const;

    /** Returns all keys of the tabVal Map
     * @brief tabValsKeys
     * @return
     */
    QStringList tabValsKeys();

    /**
     * @brief returns minimum value of 2-dimensional table
     * @param paramName parameter name
     * @param ok control flag
     * @return
     */
    double minValue2D(QString paramName, bool* ok = Q_NULLPTR);

    /**
     * @brief returns maximum value of 2-dimensional table
     * @param paramName parameter name
     * @param ok control flag
     * @return
     */
    double maxValue2D(QString paramName, bool* ok = Q_NULLPTR);

    /**
     * @brief returns minimum value of 1-dimensional table
     * @param paramName parameter name
     * @param ok control flag
     * @return minimum value of 1-dimensional table
     */
    double minValue1D(QString paramName, bool* ok = Q_NULLPTR);

    /**
     * @brief returns maximum value of 1-dimensional table
     * @param paramName parameter name
     * @param ok control flag
     * @return maximum value of 1-dimensional table
     */
    double maxValue1D(QString paramName, bool* ok = Q_NULLPTR);

    /**
     * @brief setData1Dfrom2DDataZone
     *  SetData1D with values from a 2D-DataZone
     * @param dataZone2D
     *  the 2D-Datazone to get the values from
     * @param fixedAxisNumber
     *  the axis, which is fixed to reduce the dimension
     * @param fixedAxisTick
     *  the axis tick of the fixed axis to use to select the values
     */
    void setData1Dfrom2DDataZone(GtDataZone* dataZone2D, int fixedAxisNumber,
                                 int fixedAxisTick);
    
     /**
     * @brief clears current data
     */
    void clearData();
private:

    /**
     * @brief returns table object
     * @return table object
     */
    GtTable* table();

    /**
     * @brief checks if data zone is currently valid
     * @return
     */
    bool isValid();

    /**
     * @brief isValid
     * @param ticks
     * @param vals
     * @return
     */
    bool isValid(QVector<double> ticks, QVector<double> vals);

    /**
     * @brief isValid
     * @param ticks1
     * @param ticks2
     * @param vals
     * @return
     */
    bool isValid(QVector<double> ticks1,
                 QVector<double> ticks2,
                 QVector<double> vals);

    /// data zone description
    QString m_description;
};

#endif // GTDATAZONE_H
