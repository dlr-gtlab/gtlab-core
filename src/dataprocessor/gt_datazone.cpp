/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include "gt_datazone.h"
#include "gt_table.h"
#include "gt_tableaxis.h"
#include "gt_logging.h"
#include "gt_algorithms.h"

#if GT_H5
#include "gt_h5file.h"
#include "gt_h5dataset.h"
#include "gt_h5data.h"
#endif

namespace
{
    template<typename T, typename ErrorCode>
    T valueAndSetError(T&& t, ErrorCode error, ErrorCode* toSet)
    {
        if (toSet) *toSet = error;
        return std::forward<T>(t);
    }

    template<typename T>
    T success(T&& t, bool* ok)
    {
        return valueAndSetError(std::forward<T>(t), true, ok);
    }

    template<typename T>
    T error(T&& t, bool* ok)
    {
        return valueAndSetError(std::forward<T>(t), false, ok);
    }
}

GtDataZone::GtDataZone()
{
    GtTable* table = new GtTable();
    table->setObjectName("Table");
    table->setDefault(true);
    appendChild(table);

    setFlag(UserDeletable);
}

bool
GtDataZone::doFetchData()
{
#if GT_H5
    GtTable* table = this->table();
    if (!table)
    {
        gtError() << "HDF5: Could not read from the dataset! (null table)";
        return false;
    }

    // retrieve rows and cols of table
    uint rows = m_params.length();
    uint cols = GtH5DataSpace::sum(table->dimensions());

    // retrieve table values
    QList<GtTableValues*> tabVals = table->findDirectChildren<GtTableValues*>();

    if (tabVals.length() != rows)
    {
        gtError() << "HDF5: Could not read from the dataset! "
                     "(incompatible dimensions)";
        return false;
    }

    // open the associated dataset
    GtH5File file;
    GtH5DataSet dataset;
    if (!getDataSet(file, dataset))
    {
        gtError() << "HDF5: Could not open the dataset!";
        return false;
    }

    // check for equal dimensions
    if (dataset.dataSpace() != GtH5DataSpace({ rows, cols }))
    {
        gtError() << "HDF5: Could not read from the dataset! "
                     "(incompatible dimensions)";
        return false;
    }

    // read the data from the dataset
    GtH5Data<double> values;

    if (!dataset.read(values))
    {
        gtError() << "HDF5: Could not read from the dataset!";
        return false;
    }

    // deserialize the data
    for (uint row = 0; row < rows; ++row)
    {
        tabVals.at(row)->setValues(values.data().mid(row * cols, cols));
    }

    return true;
#else
    return true;
#endif
}

bool
GtDataZone::doExternalizeData()
{
#if GT_H5
    GtTable* table = this->table();
    if (!table)
    {
        gtError() << "HDF5: Could not write to the dataset! (null table)";
        return false;
    }

    // retrieve rows and cols of table
    uint rows = m_params.length();
    uint cols = GtH5DataSpace::sum(table->dimensions());

    // serialize the data
    GtH5Data<double> values;
    values.reserve(rows * cols);

    for (auto* tabVal : table->findDirectChildren<GtTableValues*>())
    {
        values.append(tabVal->values());
    }

    if (values.length() != rows * cols)
    {
        gtError() << "HDF5: Could not write to the dataset! "
                     "(incompatible dimensions)";
        return false;
    }

    // open the associated dataset
    GtH5File file;
    GtH5DataSet dataset;
    if (!createDataSet(file, dataset, values, GtH5DataSpace{ rows, cols }))
    {
        gtError() << "HDF5: Could not open the dataset!";
        return false;
    }

    // write the data to the dataset
    if (!dataset.write(values))
    {
        gtError() << "HDF5: Could not write to the dataset!";
        return false;
    }

    return true;
#else
    return true;
#endif
}

void
GtDataZone::doClearExternalizedData()
{
    if (!table())
    {
        return;
    }

    for (auto* tableVal : table()->findDirectChildren<GtTableValues*>())
    {
        tableVal->clearValues();
    }
}

int
GtDataZone::nDims() const
{
    return table()->nDims();
}

void
GtDataZone::addModuleName(const QString& suffix)
{
    if (suffix == "")
    {
        return;
    }

    table()->setTabValsKeysSuffix(suffix + ".");

    QStringList newParamNames;

    foreach (QString param, params())
    {
        newParamNames.append(suffix + "." + param);
    }

    m_params = newParamNames;
}

GtTable*
GtDataZone::table() const
{
    return findDirectChild<GtTable*>("Table");
}

double
GtDataZone::value1D(const QString& param, const double &x0, bool* ok) const
{
    if (nDims() != 1)
    {
        gtWarning() << tr("Trying to get a 1D value out of a non-1D Table.");
        return error(0.0, ok);
    }

    GtTable* tab = table();
    if (!tab)
    {
        return error(0.0, ok);
    }

    if (!param.isEmpty() && tab->tabValsKeys().contains(param))
    {
        try
        {
            return success(tab->getValue1D(param, x0), ok);
        }
        catch (GTlabException& /*e*/) {
            return error(0.0, ok);
        }
    }

    return error(0.0, ok);
}

QVector<double>
GtDataZone::value1DVector(const QString &param, bool *ok) const
{
    if (nDims() != 1)
    {
        gtWarning() << tr("Trying to get a 1D value out of a non-1D Table.");
        return error(QVector<double>(), ok);
    }

    GtTable* tab = table();
    if (!tab)
    {
        return error(QVector<double>(), ok);
    }

    if (!param.isEmpty() && tab->tabValsKeys().contains(param))
    {
        return success(tab->getVals(param)->values(), ok);
    }

    return error(QVector<double>(), ok);
}

QVector<double>
GtDataZone::value1DVector(const QString &param, const QVector<double>& ticks, bool *ok) const
{
    if (nDims() != 1)
    {
        gtWarning() << tr("Trying to get a 1D value out of a non-1D Table.");
        return error(QVector<double>(), ok);
    }

    GtTable* tab = table();
    if (!tab)
    {
        return error(QVector<double>(), ok);
    }

    if (!ticks.isEmpty())
    {
        return error(QVector<double>(), ok);
    }

    if (!param.isEmpty() && tab->tabValsKeys().contains(param))
    {
        bool success = true;

        QVector<double> retVal;
        for (const double& tick : ticks)
        {

            double val = 0.0;

            try
            {
                val = tab->getValue1D(param, tick);
            }
            catch (GTlabException& /*e*/)
            {
                success = false;
            }
            retVal.append(val);
        }

        return ::valueAndSetError(retVal, success, ok);
    }

    return QVector<double>();
}


double
GtDataZone::value2D(const QString& param, const double& x0,
                    const double& x1, bool* ok) const
{

    if (nDims() != 2)
    {
        gtWarning() << tr("Trying to get a 2D value out of a non-2D Table.");
        return error(0.0, ok);
    }

    GtTable* tab = table();
    if (!tab)
    {
        return error(0.0, ok);
    }

    if (!param.isEmpty() && tab->tabValsKeys().contains(param))
    {
        try
        {
            return success(tab->getValue2D(param, x0, x1), ok);
        }
        catch (GTlabException& /*e*/) {
            return error(0.0, ok);
        }

    }

    return error(0.0, ok);
}

double
GtDataZone::value3D(const QString& param, const double& x0, const double& x1,
                    const double& x2, bool *ok) const
{
    if (nDims() != 3)
    {
        gtWarning() << tr("Trying to get a 3D value out of a non-3D Table.");
        return error(0.0, ok);
    }

    GtTable* tab = table();
    if (!tab)
    {
        return error(0.0, ok);
    }

    if (!param.isEmpty() && tab->tabValsKeys().contains(param))
    {
        try
        {
            return success(tab->getValue3D(param, x0, x1, x2), ok);
        }
        catch (GTlabException& /*e*/) {
            return error(0.0, ok);
        }

    }

    return error(0.0, ok);
}

double
GtDataZone::value4D(const QString& param, const double& x0, const double& x1,
                    const double& x2, const double& x3,
                    bool *ok) const
{
    if (nDims() != 4)
    {
        gtWarning() << tr("Trying to get a 4D value out of a non-4D Table.");
        return error(0.0, ok);
    }

    GtTable* tab = table();
    if (!tab)
    {
        return error(0.0, ok);
    }

    if (!param.isEmpty() && tab->tabValsKeys().contains(param))
    {
        return success(tab->getValue4D(param, x0, x1, x2, x3), ok);
    }

    return error(0.0, ok);
}

void
GtDataZone::setData1D(const QStringList& params,
                      const QVector<double>& ticks,
                      const QString& axisName1,
                      const QMap< QString, QVector<double> >& vals,
                      const QStringList& units)
{
    clearData();

    if (params.empty() || params.size() != vals.size())
    {
        gtWarning() << tr("Param sizes do not match in DataZone!");
        return;
    }


    table()->addAxis(axisName1, "", "[-]", GtTableAxis::E_LINEAR,
                 GtTableAxis::I_LINEAR, GtTableAxis::E_LINEAR,
                 ticks);

    for (int i = 0; i < params.size(); i++)
    {
        if (!isValid(ticks, vals.value(params.at(i))))
        {
            gtWarning() << tr("Ticks' size do not match values' size!");
            clearData();
            return;
        }

        table()->addValues(params.at(i), "", units.at(i),
                           vals.value(params.at(i)));
        m_params.append(params.at(i));
        m_units.append(units.at(i));
    }

    isValid();
}

void
GtDataZone::setData1D(const QStringList& params,
                      const QMap<double, QVector<double> >& vals,
                      const QString& axisName1,
                      const QStringList& units)
{
    clearData();

    if (params.empty() || params.size() != vals.first().size())
    {
        gtWarning() << tr("Param sizes do not match in DataZone!");
        gtInfo() << "Params:" << params << "Values:" << vals.first().size();
        return;
    }

    if (units.size() != params.size())
    {
        gtWarning() << tr("Unit sizes do not match in DataZone!");
        return;
    }

    if (std::any_of(std::begin(vals), std::end(vals), [&params](const QVector<double>& vec) {
            return vec.size() != params.size();
        })) {
        gtWarning() << tr("Value sizes do not match in DataZone!");
        return;
    }

    QVector<double> ticks;
    QVector< QVector<double> > newVals;
    newVals.resize(params.size());

    // loop through vals to sort them to
    //           t1,   t2,   t3
    // param1: val1, val2, val3
    // param2: val1, val2, val3
    // param3: val1, val2, val3

    for_each_key (vals, [&](double key)
    {
        ticks.append(key);

        for (int j = 0; j < params.size(); j++)
        {
            double val = vals.value(key).at(j);
            newVals[j].append(val);
        }
    });

    table()->addAxis(axisName1, "", "[-]", GtTableAxis::E_LINEAR,
                 GtTableAxis::I_LINEAR, GtTableAxis::E_LINEAR,
                 ticks);

    for (int i = 0; i < params.size(); i++)
    {
        if (!isValid(ticks, newVals.at(i)))
        {
            gtWarning() << tr("Ticks' size do not match values' size!");
            clearData();
            return;
        }

        table()->addValues(params.at(i), "", units.at(i), newVals.at(i));
        m_params.append(params.at(i));
        m_units.append(units.at(i));
    }

    isValid();
}

void
GtDataZone::setData1Dfrom2DDataZone(GtDataZone* dataZone2D, int fixedAxisNumber,
                                    int fixedAxisTick)
{
    if (!dataZone2D)
    {
        return;
    }

    if (dataZone2D->nDims() != 2)
    {
        gtError() << "Cannot use setData1Dfrom2DDataZone with a Datazone "
                     "of dimension" << dataZone2D->nDims();
    }

    if (fixedAxisNumber != 0 && fixedAxisNumber != 1)
    {
        return;
    }

    QStringList params = dataZone2D->params();
    QStringList units = dataZone2D->units();


    QVector<double> ticks;
    QString axisName;


    double fixedAxisValue;
    if (fixedAxisNumber == 0)
    {
        axisName = dataZone2D->axisNames().at(1);
        ticks = dataZone2D->allAxisTicks().at(1);

        if (fixedAxisTick >= allAxisTicks().at(0).size())
        {
            return;
        }

        fixedAxisValue = allAxisTicks().at(0).at(fixedAxisTick);
    }
    else if (fixedAxisNumber == 1)
    {
        axisName = dataZone2D->axisNames().at(0);
        ticks = dataZone2D->allAxisTicks().at(0);

        if (fixedAxisTick >= allAxisTicks().at(1).size())
        {
            return;
        }
        fixedAxisValue = allAxisTicks().at(1).at(fixedAxisTick);
    }

    QMap< QString, QVector<double> > vals;

    for (const QString& param : qAsConst(params))
    {
        QVector<double> currentVals;

        for (int k = 0; k < ticks.size(); ++k)
        {
            double value = 0;
            if (fixedAxisNumber == 0)
            {
                value = dataZone2D->value2D(param, fixedAxisValue, ticks.at(k));
            }
            else if (fixedAxisNumber == 1)
            {
                value = dataZone2D->value2D(param, ticks.at(k), fixedAxisValue);
            }
            currentVals.append(value);
        }

        vals.insert(param, currentVals);
    }

    setData1D(params, ticks, axisName, vals, units);
}


void
GtDataZone::setData2D(const QStringList& params,
                      const QVector<double>& ticks1,
                      const QVector<double>& ticks2,
                      const QString& axisName1,
                      const QString& axisName2,
                      const QMap<QString, QVector<double> > &vals,
                      const QStringList &units)
{
    clearData();

    if (params.empty() || params.size() != vals.size())
    {
        gtWarning() << tr("Param sizes do not match in DataZone!");
        return;
    }

    GtTable* t = table();

    t->addAxis(axisName1, "", "[-]", GtTableAxis::E_LINEAR,
                 GtTableAxis::I_LINEAR, GtTableAxis::E_LINEAR,
                 ticks1);

    t->addAxis(axisName2, "", "[-]", GtTableAxis::E_LINEAR,
                 GtTableAxis::I_LINEAR, GtTableAxis::E_LINEAR,
                 ticks2);

    for (int i = 0; i < params.size(); i++)
    {
        if (!isValid(ticks1, ticks2, vals.value(params.at(i))))
        {
            gtWarning() << tr("Ticks' size do not match values' size "
                              "in DataZone!");
            clearData();
            return;
        }

        t->addValues(params.at(i), "", units.at(i), vals.value(params.at(i)));
        m_params.append(params.at(i));
        m_units.append(units.at(i));
    }

    isValid();
}

void
GtDataZone::clearData()
{
//    delete table();

    if (table())
    {
        table()->clear();
    }

    m_units.clear();
    m_params.clear();
}

bool
GtDataZone::isValid(const QVector<double>& ticks,
                    const QVector<double>& vals) const
{
    if (ticks.size() != vals.size())
    {
        gtWarning() << tr("Tick-Sizes do not match in DataZone!");
        return false;
    }

    return true;
}

bool
GtDataZone::isValid(const QVector<double> &ticks1,
                    const QVector<double> &ticks2,
                    const QVector<double> &vals) const
{
    if (ticks1.size() * ticks2.size() != vals.size())
    {
        gtWarning() << tr("Tick-Sizes do not match in DataZone!");
        return false;
    }

    return true;
}

QString
GtDataZone::description() const
{
    return m_description;
}

QStringList
GtDataZone::tabValsKeys() const
{
    GtTable* t = table();

    if (!t) return QStringList();

    return t->tabValsKeys();
}

void
GtDataZone::setDescription(const QString &description)
{
    m_description = description;
}

bool
GtDataZone::isValid() const
{
    if (m_params.size() != table()->tabValsKeys().size())
    {
        gtWarning() << tr("Param does not match table in DataZone!");
        gtDebug() << tr("Params:") << m_params.size()
                  << tr("Keys:") << table()->tabValsKeys().size();

        gtDebug() << "Params: " << m_params;
        gtDebug() << "Keys: " << table()->tabValsKeys();
        return false;
    }

    if (m_params.size() != m_units.size())
    {
        gtWarning() << tr("Param does not match size of units!");
        return false;
    }

    return true;
}


QString
GtDataZone::unit(const QString& param) const
{
    int index = m_params.indexOf(param);

    if (index == -1)
    {
        gtDebug() << tr("Param ") << param
                  << tr(" could not be found in n-D data, "
                        "no unit can be shown");
        return QString();
    }

    return m_units.at(index);
}

void
GtDataZone::axisTicks(const QString& id, QVector<double> &axTicks) const
{
    if (axisNames().contains(id))
    {
        table()->getAxisTicks(id, axTicks);
    }
    else
    {
        gtWarning() << tr("DataZone does not contain Axis '") << id << "'";
    }
}

void
GtDataZone::axisTicks(const QString &id, QStringList& axTicks) const
{
    if (axisNames().contains(id))
    {
        QVector<double> ticks;
        table()->getAxisTicks(id, ticks);

        axTicks.clear();
        for (const double& tick : qAsConst(ticks))
        {
            axTicks.append(QString::number(tick));
        }
    }
    else
    {
        gtWarning() << tr("DataZone does not contain Axis '") << id << "'";
    }
}

QVector<double>
GtDataZone::axisTicks(const QString& id) const
{
    QVector<double> retVal;

    axisTicks(id, retVal);

    return retVal;
}

QStringList
GtDataZone::axisTickStrings(const QString &id) const
{
    QStringList retVal;

    axisTicks(id, retVal);

    return retVal;
}

QStringList
GtDataZone::axisTickLabels(const QString& id) const
{
    QStringList retval;

    auto ticks = axisTicks(id);
    for (const double& val : qAsConst(ticks))
    {
        retval.append(id + " = " + QString::number(val));
    }

    return retval;
}

QVector<QVector<double> >
GtDataZone::allAxisTicks() const
{
    QVector< QVector<double> > retval;

    auto names = axisNames();
    for (const QString& str : qAsConst(names))
    {
        QVector<double> vec = axisTicks(str);
        retval.push_back(vec);
    }

    return retval;
}

QMap<QString, QVector<double> >
GtDataZone::allAxisTicksMap() const
{
    QMap<QString, QVector<double> > retval;

    auto names = axisNames();
    for (const QString& str : qAsConst(names))
    {
        QVector<double> vec = axisTicks(str);

        retval.insert(str, vec);
    }

    return retval;
}

QStringList
GtDataZone::axisNames() const
{
    return table()->getAxesNames();
}

bool
GtDataZone::is0D() const
{
    return false;
}

double
GtDataZone::minValue2D(const QString &paramName, bool* ok)
{
    double retVal = qPow(10, 20);

    // @todo: this seems to be wrong. Issue #195 is created
    if (ok)
    {
        *ok = false;
        return retVal;
    }

    QVector<double> firstAxisTicks = axisTicks(axisNames().first());
    QVector<double> secondAxisTicks = axisTicks(axisNames().last());

    for (const double& val1 : qAsConst(firstAxisTicks))
    {
        for (const double& val2 : qAsConst(secondAxisTicks))
        {
            double val = value2D(paramName, val1, val2);

            if (val < retVal)
            {
                retVal = val;
            }
        }
    }

    return success(retVal, ok);
}

double
GtDataZone::maxValue2D(const QString& paramName, bool* ok)
{
    double retVal = -qPow(10, 20);

    // @todo: this seems to be wrong. Issue #195 is created
    if (ok)
    {
        *ok = false;
        return retVal;
    }

    QVector<double> firstAxisTicks = axisTicks(axisNames().first());
    QVector<double> secondAxisTicks = axisTicks(axisNames().last());

    for (const double& val1 : qAsConst(firstAxisTicks))
    {
        for (const double& val2 : qAsConst(secondAxisTicks))
        {
            double val = value2D(paramName, val1, val2);

            if (val > retVal)
            {
                retVal = val;
            }
        }
    }

    return success(retVal, ok);
}

double
GtDataZone::minValue1D(const QString &paramName, bool *ok)
{
    double retVal = qPow(10, 20);

    // @todo: this seems to be wrong. Issue #195 is created
    if (ok)
    {
        *ok = false;
        return retVal;
    }

    QVector<double> firstAxisTicks = axisTicks(axisNames().first());

    for (const double& val1 : qAsConst(firstAxisTicks))
    {
        double val = value1D(paramName, val1);

        if (val < retVal)
        {
            retVal = val;
        }
    }

    return success(retVal, ok);
}

double
GtDataZone::maxValue1D(const QString& paramName, bool *ok)
{
    double retVal = -qPow(10, 20);

    // @todo: this seems to be wrong. Issue #195 is created
    if (ok)
    {
        *ok = false;
        return retVal;
    }

    QVector<double> firstAxisTicks = axisTicks(axisNames().first());

    for (const double& val1 : qAsConst(firstAxisTicks))
    {
        double val = value1D(paramName, val1);

        if (val > retVal)
        {
            retVal = val;
        }
    }

    return success(retVal, ok);
}
