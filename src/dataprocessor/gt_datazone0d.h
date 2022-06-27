/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#ifndef GTDATAZONE0D_H
#define GTDATAZONE0D_H

#include "gt_abstractdatazone.h"

#include <QVector>
#include <QStringList>


class GtDataZone0D;
class GT_DATAMODEL_EXPORT GtDataZone0DData : public GtAbstractDataZoneData
{
    GT_DECL_BASECLASS(GtDataZone0D)

public:

    explicit GtDataZone0DData(GtDataZone0D* base);

    /**
     * @brief Returns true if size of params, units and values matches.
     * @return Data validity indicator.
     */
    bool isValid() const override;

    /**
     * @brief Returns the value assigned to the given parameter name (if existent)
     *        If the parameter name is not contained in the current parameter map
     *        a value of 0.0 will be returned!
     *        Check bool parameter for success!
     * @param paramName
     * @param ok
     * @return
     */
    double value(const QString& paramName, bool* ok = nullptr) const;

    /**
     * @brief values
     * @return values
     */
    const QVector<double>& values() const;

    /**
     * @brief setValues
     * @param values
     */
    void setValues(const QVector<double>& values);

    /**
     * @brief If the given parameter name exists in m_parameterMap its current
     *        value will be replaced by the given value
     * @param paramName
     * @param value
     * @return
     */
    bool setValue(const QString& paramName,
                  const double& value);

    /**
     * @brief Applies a new set of parameters and values to the data zone
     *        Old Data will be deleted!
     * @param paramNames
     * @param values
     * @param units
     * @return
     */
    bool setData(const QStringList& paramNames,
                 const QVector<double>& values,
                 const QStringList& units);

    /**
     * @brief if the given parameter name is not yet contained in the current
     *        parameter map, it will be inserted and assigned to the given value
     *        The default unit will be "-"
     * @param paramName - name of the parameter to append or overwrite
     * @param value - value to use
     * @param overwrite - if true if the parameter already exist the value and
     * unit will be overwritten.
     * If the parameter exists and "overwrite" is false the function
     * will not change the data and return false
     * @return true in case of success
     */
    bool appendData(const QString& paramName,
                    const double& value,
                    bool overwrite = true);

    /**
     * @brief Function setValue(const QString &paramName, const double &value)
     *         will be performed for all parameter names
     * If a parameter already exists in the datazone
     * its value will be overwritten
     * @param paramNames - list of all parameters to add or update
     * @param values - values of all parameters to append or update
     * Both lists have to be of the same size
     * @return true in case of success
     */
    bool appendData(const QList<QString>& paramNames,
                    const QVector<double>& values);

    /**
     * @brief Function setValue(const QString &paramName, const double &value)
     *         will be performed for all parameter names
     * If a parameter already exists in the datazone
     * its value will be overwritten
     * @param paramNames - list of all parameters to add or update
     * @param units - list of all units of all parameters
     * @param values - values of all parameters
     * Both lists have to be of the same size
     * @return true in case of success
     */
    bool appendData(const QList<QString>& paramNames,
                    const QList<QString>& units,
                    const QVector<double>& values);

    /**
     * @brief appendData
     * Appends an element to the datazone or overwrites an existing one
     * @param name - name of the parameter to add or overwrite
     * @param unit - unit of the parameter
     * @param val - value of the parameter
     * @param overwrite - if true if the parameter already exist the value and
     * unit will be overwritten.
     * If the parameter exists and "overwrite" is false the function
     * will not change the data and return false
     * @return true in case of success
     */
    bool appendData(const QString& name,
                    const QString& unit,
                    const double& val,
                    bool overwrite = true);

    /**
     * @brief clearData: Clears parameter map
     */
    void clearData();
};


/**
 * @brief The GtDataZone0D class
 */
class GT_DATAMODEL_EXPORT GtDataZone0D : public GtAbstractDataZone
{
    Q_OBJECT

    Q_PROPERTY(QVector<double> values MEMBER m_values)

    GT_DECL_DATACLASS(GtDataZone0DData)

public:

    /**
     * @brief GtDataZone0D
     */
    Q_INVOKABLE GtDataZone0D();

    /**
     * @brief is0D
     * @return true
     */
    bool is0D() const override;

    /**
     * @brief nDims
     * @return 0
     */
    int nDims() const override;

protected:

    /**
     * @brief fetchs the externalized params, units and values properties.
     * @param metaData metadata for fetching.
     * @param fetchInitialVersion whether to fetch the intial data version
     * @return success
     */
    bool doFetchData(QVariant& metaData, bool fetchInitialVersion) override;

    /**
     * @brief externalizes the params, units and values properties.
     * @param metaData metadata for externalizing.
     * @return success
     */
    bool doExternalizeData(QVariant& metaData) override;

    /**
     * @brief clears the params, values and units
     */
    void doClearExternalizedData() override;

private:

    /// Parameter values
    QVector<double> m_values;
};

#endif // GTDATAZONE0D_H
