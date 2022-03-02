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

#include "gt_datamodel_exports.h"

#include <QVector>
#include <QHash>
#include <QStringList>

#include "gt_abstractdatazone.h"

/**
 * @brief The GtDataZone0D class
 */
class GT_DATAMODEL_EXPORT GtDataZone0D : public GtAbstractDataZone
{
    Q_OBJECT

    Q_PROPERTY(QVector<double> values READ values WRITE setValues)

public:
    /**
     * @brief GtDataZone0D
     */
    Q_INVOKABLE GtDataZone0D();

    /**
     * @brief Applies a new set of parameters and values to the data zone
     *        Old Data will be deleted!
     * @param paramNames
     * @param values
     * @param units
     * @return
     */
    bool setData(const QStringList &paramNames,
                 const QVector<double>& values, const QStringList &units);

    /**
     * @brief If the given parameter name exists in m_parameterMap its current value
     *        will be replaced by the given value
     * @param paramName
     * @param value
     * @return
     */
    bool setValue(const QString& paramName,
                  const double& value);

    /**
     * @brief if the given parameter name is not yet contained in the current
     *        parameter map, it will be inserted and assigned to the given value
     * @param paramName
     * @param value
     * @return
     */
    bool appendData(const QString& paramName,
                    const double& value);

    /**
     * @brief Function setValue(const QString &paramName, const double &value)
     *         will be performed for all parameter names
     * @param paramNames
     * @param values
     * @return
     */
    bool appendData(const QList<QString>& paramNames,
                    const QVector<double>& values);

    /**
     * @brief appendData
     * @param name
     * @param unit
     * @param val
     * @return
     */
    bool appendData(const QString& name,
                    const QString& unit,
                    const double& val);

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
     * @brief unitFromParam
     * @return
     */
    virtual QString unit(const QString& param) const override;

    /**
     * @brief values
     * @return
     */
    QVector<double> values() const;

    /**
     * @brief setValues
     * @param values
     */
    void setValues(const QVector<double>& values);

    /**
     * @brief is0D
     * @return
     */
    virtual bool is0D() const override;

    /**
     * @brief nDims
     * @return
     */
    virtual int nDims() const override;

    /**
     * @brief addModuleName
     * @param moduleName
     */
    void addModuleName(const QString& moduleName) override;

    /**
     * @brief Returns true if size of params, units and values matches.
     * @return Data validity indicator.
     */
    bool isValid() const;

    /**
     * @brief clearData: Clears parameter map
     */
    void clearData();

protected:

    /**
     * @brief doFetch fetches the externalized data.
     * @return success
     */
    bool doFetchData() override;

    /**
     * @brief doExternalize externalizes the fetched data.
     * @return success
     */
    bool doExternalizeData() override;

    /**
     * @brief doClearExternalizedData clears the data.
     */
    void doClearExternalizedData() override;

private:

    /// Parameter values
    QVector<double> m_values;

};

#endif // GTDATAZONE0D_H
