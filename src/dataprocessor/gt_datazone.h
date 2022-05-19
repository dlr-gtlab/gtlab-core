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

#include "gt_abstractdatazone.h"

class GtTable;
class GtDataZone;
class GT_DATAMODEL_EXPORT GtDataZoneData : public GtAbstractDataZoneData
{
    GT_DECL_BASECLASS(GtDataZone)

public:

    explicit GtDataZoneData(GtDataZone* base);

    /**
     * @brief checks if data zone is currently valid
     * @return
     */
    bool isValid() const override;

    /**
     * @brief addModuleName
     * @param moduleName
     */
    void addModuleName(const QString& suffix) override;

    /**
     * @brief returns vector of values for 1D data zone based on the
     * defined axis ticks
     * @param param parameter name
     * @param ok control flag
     * @return
     */
    QVector<double> value1DVector(const QString& param,
                                  bool* ok = nullptr) const;

    /**
     * @brief returns vector of values for 1D data zone based on the
     * defined axis ticks in the argument vector
     * @param param parameter name
     * @param ticks axis ticks
     * @param ok control flag
     * @return
     */
    QVector<double> value1DVector(const QString& param,
                                  const QVector<double>& ticks,
                                  bool* ok = nullptr) const;

    /**
     * @brief set 1-dimensional data to table
     * @param params parameter names
     * @param ticks axis ticks
     * @param axisName1 axis name
     * @param vals parameter names and values
     * @param units parameter units
     */
    void setData1D(const QStringList& params,
                   const QVector<double>& ticks,
                   const QString& axisName1,
                   const QMap<QString, QVector<double> >& vals,
                   const QStringList& units);

    /**
     * @brief set 1-dimensional data to table
     * @param params parameter names
     * @param vals axis ticks and values
     * @param axisName1 axis name
     * @param units parameter units
     */
    void setData1D(const QStringList& params,
                   const QMap<double, QVector<double> >& vals,
                   const QString& axisName1,
                   const QStringList& units);

    /**
     * @brief addDataPoint1D
     * @param vals - map of values to set in the datazone
     * @param tick - main axis value to add the parameter values to
     * @param overwrite -  if the tick is existing the existing values
     * will be overwritten if this flag is set to true (default value is true)
     */
    bool addDataPoint1D(const QMap<QString, double>& vals,
                        const double& tick,
                        bool overwrite = true);

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
    void setData2D(const QStringList& params,
                   const QVector<double>& ticks1,
                   const QVector<double>& ticks2,
                   const QString& axisName1,
                   const QString& axisName2,
                   const QMap<QString, QVector<double> >& vals,
                   const QStringList& units);

    /**
     * @brief Returns value from 1-dimensional datazone
     * @param param parameter name
     * @param x0 axis tick
     * @param ok control flag
     * @return value
     */
    double value1D(const QString& param, const double& x0,
                   bool* ok = nullptr) const;

    /**
     * @brief Returns value from 2-dimensional table
     * @param param parameter name
     * @param x0 first dimension axis tick
     * @param x1 second dimension axis tick
     * @param ok control flag
     * @return value
     */
    double value2D(const QString& param, const double& x0,
                   const double& x1, bool* ok = nullptr) const;

    /**
     * @brief Returns value from 3-dimensional table
     * @param param parameter name
     * @param x0 first dimension axis tick
     * @param x1 second dimension axis tick
     * @param x2 third dimension axis tick
     * @param ok control flag
     * @return value
     */
    double value3D(const QString& param, const double& x0,
                   const double& x1, const double& x2,
                   bool* ok = nullptr) const;

    /**
     * @brief Returns value from 4-dimensional table
     * @param param parameter name
     * @param x0 first dimension axis tick
     * @param x1 second dimension axis tick
     * @param x2 third dimension axis tick
     * @param x3 fourth dimensio axis tick
     * @param ok control flag
     * @return value
     */
    double value4D(const QString& param, const double& x0,
                   const double& x1, const double& x2,
                   const double& x3, bool* ok = nullptr) const;

    /**
     * @brief returns minimum value of 2-dimensional table
     * @param paramName parameter name
     * @param ok control flag
     * @return minimum value
     */
    double minValue2D(const QString& paramName, bool* ok = nullptr);

    /**
     * @brief returns maximum value of 2-dimensional table
     * @param paramName parameter name
     * @param ok control flag
     * @return maximum value
     */
    double maxValue2D(const QString& paramName, bool* ok = nullptr);

    /**
     * @brief returns minimum value of 1-dimensional table
     * @param paramName parameter name
     * @param ok control flag
     * @return minimum value of 1-dimensional table
     */
    double minValue1D(const QString& paramName, bool* ok = nullptr);

    /**
     * @brief returns maximum value of 1-dimensional table
     * @param paramName parameter name
     * @param ok control flag
     * @return maximum value of 1-dimensional table
     */
    double maxValue1D(const QString& paramName, bool* ok = nullptr);

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
    void setData1Dfrom2DDataZone(GtDataZone* dataZone2D,
                                 int fixedAxisNumber,
                                 int fixedAxisTick);

    /**
    * @brief clears current data
    */
   void clearData();

private:

    /**
     * @brief Checks if size of ticks is equal to values
     * @param ticks ticks
     * @param vals param values
     * @return valid sized ticks and vals
     */
    static bool isValid(const QVector<double>& ticks,
                        const QVector<double>& vals);

    /**
     * @brief Checks if size of both ticks is equal to values
     * @param ticks1 first ticks
     * @param ticks2 seconds ticks
     * @param vals param values
     * @return valid sized ticks and vals
     */
    static bool isValid(const QVector<double>& ticks1,
                        const QVector<double>& ticks2,
                        const QVector<double>& vals);
};

/**
 * @brief The GtDataZone class
 */
class GT_DATAMODEL_EXPORT GtDataZone : public GtAbstractDataZone
{
    Q_OBJECT

    Q_PROPERTY(QString description READ description WRITE setDescription)

    GT_DECL_DATACLASS(GtDataZoneData)

public:

    /**
     * @brief GtDataZone
     */
    Q_INVOKABLE GtDataZone();

    /**
     * @brief setDescription
     * @param description
     */
    void setDescription(const QString& description);

    /**
     * @brief returns data zone description
     * @return data zone description
     */
    const QString& description() const;

    /** Returns all keys of the tabVal Map
     * @brief tabValsKeys
     * @return
     */
    QStringList tabValsKeys() const;

    /**
     * @brief stores axis ticks in argument vector
     * @param id axis id
     * @param axTicks axis ticks vector
     */
    void axisTicks(const QString& id, QVector<double>& axTicks) const;

    /**
     * @brief returns axis ticks in argument string list
     * @param id axis id
     * @param axTicks axis ticks string list
     */
    void axisTicks(const QString& id, QStringList& axTicks) const;

    /**
     * @brief returns axis ticks as vector of type double
     * @param id axis id
     * @return axis ticks
     */
    QVector<double> axisTicks(const QString& id) const;

    /**
     * @brief returns axis ticks as vector of type double
     * @param axisIndex - index of the axis (default value is 0 for main axis)
     * @return axis ticks
     */
    QVector<double> axisTicks(int axisIndex = 0) const;

    /**
     * @brief isOD
     * @return false
     */
    bool is0D() const override;

   /**
    * @brief returns the number of dimensions
    * @return number of dimensions
    */
   int nDims() const override;

    /**
     * @brief returns all axis names
     * @return axis names
     */
    Q_INVOKABLE QStringList axisNames() const;

    /**
     * @brief returns axis ticks as list of type QString
     * @param id axis id
     * @return axis ticks
     */
    QStringList axisTickStrings(const QString& id) const;

    /**
     * @brief returns axis tick labels i.e. for plots
     * @param id axis id
     * @return axis labels
     */
    QStringList axisTickLabels(const QString& id) const;

    /**
     * @brief returns ticks of all table axis in one vector
     * @return all axis ticks
     */
    QVector< QVector<double> > allAxisTicks() const;

    /**
     * @brief returns map of axis names and approriate ticks in form of a vector
     * @return all axis names and ticks
     */
    QMap<QString, QVector<double> > allAxisTicksMap() const;

    /**
     * @brief Returns value from 1-dimensional datazone.
     * Intended for python interface only, use data object instead.
     * @param param parameter name
     * @param x0 axis tick
     * @param ok control flag
     * @return value
     */
    Q_INVOKABLE double pyValue1D(const QString& param, const double& x0,
                                 bool* ok = nullptr);

    /**
     * @brief Returns value from 2-dimensional table.
     * Intended for python interface only, use data object instead.
     * @param param parameter name
     * @param x0 first dimension axis tick
     * @param x1 second dimension axis tick
     * @param ok control flag
     * @return value
     */
    Q_INVOKABLE double pyValue2D(const QString& param, const double& x0,
                                 const double& x1, bool* ok = nullptr);

    /**
     * @brief Returns value from 3-dimensional table.
     * Intended for python interface only, use data object instead.
     * @param x0 first dimension axis tick
     * @param x1 second dimension axis tick
     * @param x2 third dimension axis tick
     * @param ok control flag
     * @return value
     */
    Q_INVOKABLE double pyValue3D(const QString& param, const double& x0,
                                 const double& x1, const double& x2,
                                 bool* ok = nullptr);

    /**
     * @brief Returns value from 4-dimensional table.
     * Intended for python interface only, use data object instead.
     * @param param parameter name
     * @param x0 first dimension axis tick
     * @param x1 second dimension axis tick
     * @param x2 third dimension axis tick
     * @param x3 fourth dimensio axis tick
     * @param ok control flag
     * @return value
     */
    Q_INVOKABLE double pyValue4D(const QString& param, const double& x0,
                                 const double& x1, const double& x2,
                                 const double& x3, bool* ok = nullptr);

protected:

    /**
     * @brief Fetchs the externalized GtTableValues.
     * @param metaData metadata for fetching.
     * @param fetchInitialVersion whether to fetch the intial version of the
     * data
     * @return success
     */
    bool doFetchData(QVariant& metaData, bool fetchInitialVersion) override;

    /**
     * @brief Externalizes the GtTableValues.
     * @param metaData metadata for externalizing.
     * @return success
     */
    bool doExternalizeData(QVariant& metaData) override;

    /**
     * @brief Clears the internal GtTableValues
     */
    void doClearExternalizedData() override;

private:

    /**
     * @brief returns table object
     * @return table object
     */
    GtTable* table() const;

    /// data zone description
    QString m_description{};
};

#endif // GTDATAZONE_H
