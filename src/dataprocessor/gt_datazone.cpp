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
#include "gt_h5externalizehelper.h"


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

    /// operator for comparing min max
    enum OperatorType { Min, Max };

    /// min or max numerical double
    constexpr double minMaxDouble(OperatorType op)
    {
        return (op == OperatorType::Max) ? std::numeric_limits<double>::min() :
                                           std::numeric_limits<double>::max();
    }

    double minFunction(double min, double val)
    {
        return std::min(min, val);
    }

    double maxFunction(double max, double val)
    {
        return std::max(max, val);
    }

    /// helper for min max operator function
    constexpr auto minMaxOperator(OperatorType op)
    {
        return (op == OperatorType::Max) ? maxFunction : minFunction;
    }

    double minMaxHelper1D(const GtDataZoneData& data,
                          const GtDataZone& dataZone,
                          const QString& paramName,
                          OperatorType op,
                          bool* ok)
    {
        auto retVal = minMaxDouble(op);
        auto opFunc = minMaxOperator(op);

        auto axisName = dataZone.axisNames();
        auto firstAxisTicks = dataZone.axisTicks(axisName.first());

        for (const double& val1 : qAsConst(firstAxisTicks))
        {
            retVal = opFunc(retVal, data.value1D(paramName, val1, ok));
        }
        return retVal;
    }

    double minMaxHelper2D(const GtDataZoneData& data,
                          const GtDataZone& dataZone,
                          const QString& paramName,
                          OperatorType op,
                          bool* ok)
    {
        auto retVal = minMaxDouble(op);
        auto opFunc = minMaxOperator(op);

        auto axisName = dataZone.axisNames();
        auto firstAxisTicks = dataZone.axisTicks(axisName.first());
        auto secondAxisTicks = dataZone.axisTicks(axisName.last());

        for (const double& val1 : qAsConst(firstAxisTicks))
        {
            for (const double& val2 :  qAsConst(secondAxisTicks))
            {
                retVal = opFunc(retVal, data.value2D(paramName, val1, val2,
                                                     ok));
            }
        }
        return retVal;
    }

    /// creates a new table and sets its parent to this datazone
    GtTable* createTable(GtDataZone& dz)
    {
        auto* table = new GtTable();
        table->setObjectName(QStringLiteral("Table"));
        table->setDefault(true);
        dz.appendChild(table);

        return table;
    }
}

GtDataZone::GtDataZone()
{
    createTable(*this);

    setFlag(UserDeletable);
}

bool
GtDataZone::doFetchData(QVariant& metaData, bool fetchInitialVersion)
{
#ifdef GT_H5
    auto* table = this->table();
    if (!table)
    {
        gtError() << tr("HDF5: Could not read from the dataset!")
                  << tr("(null table)");
        return false;
    }

    // retrieve rows and cols of table
    int rows = m_params.length();
    int cols = GtH5DataSpace::sum(table->dimensions());

    // retrieve table values
    QList<GtTableValues*> tabVals{table->findDirectChildren<GtTableValues*>()};

    if (tabVals.length() != rows)
    {
        gtError() << tr("HDF5: Could not read from the dataset!")
                  << tr("(invalid table values dimension)");
        return false;
    }

    // read the data
    GtH5ExternalizeHelper h5Worker{*this};

    GtH5Data<double> data;

    if (!h5Worker.read(data, metaData, fetchInitialVersion))
    {
        return false;
    }

    if (data.length() != rows * cols)
    {
        gtError() << tr("HDF5: Could not read from the dataset!")
                  << tr("(invalid table values dimension)");
        return false;
    }

    // deserialize the data
    for (int row = 0; row < rows; ++row)
    {
        tabVals.at(row)->setValues(data.data().mid(row * cols, cols));
    }
#endif
    return true;
}

bool
GtDataZone::doExternalizeData(QVariant& metaData)
{
#ifdef GT_H5
    auto* table = this->table();
    if (!table)
    {
        gtError() << tr("HDF5: Could not write to the dataset!")
                  << tr("(null table)");
        return false;
    }

    // retrieve rows and cols of table
    int rows{m_params.length()};
    int cols{GtH5DataSpace::sum(table->dimensions())};

    // serialize the data
    GtH5ExternalizeHelper h5Worker{*this};

    GtH5Data<double> data;
    data.reserve(rows * cols);

    for (const auto* tabVal : table->findDirectChildren<GtTableValues*>())
    {
        data.append(tabVal->values());
    }

    if (data.length() != rows * cols)
    {
        gtError() << tr("HDF5: Could not write to the dataset!")
                  << tr("(invalid table values dimension)");
        return false;
    }

    // serialize the data
    return h5Worker.write(data, metaData);
#else
    return true;
#endif
}

void
GtDataZone::doClearExternalizedData()
{
#ifdef GT_H5
    if (!table())
    {
        return;
    }

    for (auto* tableVal : table()->findDirectChildren<GtTableValues*>())
    {
        tableVal->clearValues();
    }
#endif
}

int
GtDataZone::nDims() const
{
    return table()->nDims();
}

bool
GtDataZone::is0D() const
{
    return false;
}

GtTable*
GtDataZone::table() const
{
    return findDirectChild<GtTable*>(QStringLiteral("Table"));
}

double
GtDataZoneData::value1D(const QString& param, const double& x0, bool* ok) const
{
    Q_ASSERT(m_base != nullptr);

    if (base()->nDims() != 1)
    {
        gtWarning() << QObject::tr("Trying to get a 1D value out of a non-1D"
                                   "Table.");
        return error(0.0, ok);
    }

    GtTable* tab = base()->table();
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
        catch (GTlabException& /*e*/)
        {
            return error(0.0, ok);
        }
    }

    return error(0.0, ok);
}

QVector<double>
GtDataZoneData::value1DVector(const QString& param, bool* ok) const
{
    Q_ASSERT(m_base != nullptr);

    if (base()->nDims() != 1)
    {
        gtWarning() << QObject::tr("Trying to get a 1D value out of a non-1D Table.");
        return error(QVector<double>(), ok);
    }

    GtTable* tab = base()->table();
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
GtDataZoneData::value1DVector(const QString& param,
                              const QVector<double>& ticks,
                              bool* ok) const
{
    Q_ASSERT(m_base != nullptr);

    if (base()->nDims() != 1)
    {
        gtWarning() << QObject::tr("Trying to get a 1D value out of a non-1D "
                                   "Table.");
        return error(QVector<double>(), ok);
    }

    GtTable* tab = base()->table();
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

    return error(QVector<double>(), ok);
}


double
GtDataZoneData::value2D(const QString& param, const double& x0,
                        const double& x1, bool* ok) const
{
    Q_ASSERT(m_base != nullptr);

    if (base()->nDims() != 2)
    {
        gtWarning() << QObject::tr("Trying to get a 2D value out of a non-2D Table.");
        return error(0.0, ok);
    }

    GtTable* tab = base()->table();
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
        catch (GTlabException& /*e*/)
        {
            return error(0.0, ok);
        }

    }

    return error(0.0, ok);
}

double
GtDataZoneData::value3D(const QString& param, const double& x0,
                        const double& x1, const double& x2, bool* ok) const
{
    Q_ASSERT(m_base != nullptr);

    if (base()->nDims() != 3)
    {
        gtWarning() << QObject::tr("Trying to get a 3D value out of a non-3D Table.");
        return error(0.0, ok);
    }

    GtTable* tab = base()->table();
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
        catch (GTlabException& /*e*/)
        {
            return error(0.0, ok);
        }
    }

    return error(0.0, ok);
}

double
GtDataZoneData::value4D(const QString& param, const double& x0,
                        const double& x1, const double& x2,
                        const double& x3, bool* ok) const
{
    Q_ASSERT(m_base != nullptr);

    if (base()->nDims() != 4)
    {
        gtWarning() << QObject::tr("Trying to get a 4D value out of a non-4D "
                                   "Table.");
        return error(0.0, ok);
    }

    GtTable* tab = base()->table();
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
GtDataZoneData::setData1D(const QStringList& params,
                          const QVector<double>& ticks,
                          const QString& axisName1,
                          const QMap< QString, QVector<double> >& vals,
                          const QStringList& units)
{
    Q_ASSERT(m_base != nullptr);

    if (params.isEmpty() || params.size() != vals.size() ||
            params.size() != units.size())
    {
        gtWarning() << QObject::tr("Parameter sizes do not match in DataZone!");
        gtWarning() << params.size() << "parameters vs." << vals.size()
                    << "values vs." << units.size() << "units";
        clearData();
        return;
    }

    base()->m_params = params;
    base()->m_units = units;

    auto* tab = base()->table();
    if (!tab)
    {
        tab = createTable(*base());
    }

    tab->clear();
    tab->addAxis(axisName1, {}, "[-]", GtTableAxis::E_LINEAR,
                 GtTableAxis::I_LINEAR, GtTableAxis::E_LINEAR,
                 ticks);

    for (int i = 0; i < params.size(); i++)
    {
        const auto& param = params.at(i);
        auto pVals = vals.value(param);

        if (!isValid(ticks, pVals))
        {
            gtWarning() << QObject::tr("Ticks' size do not match values' "
                                       "size!");
            clearData();
            return;
        }

        tab->addValues(param, {}, units.at(i), pVals);
    }

    isValid();
}

void
GtDataZoneData::setData1D(const QStringList& params,
                          const QMap<double, QVector<double> >& vals,
                          const QString& axisName1,
                          const QStringList& units)
{
    Q_ASSERT(m_base != nullptr);

    if (params.isEmpty() || params.size() != vals.size() ||
            params.size() != units.size())
    {
        gtWarning() << QObject::tr("Parameter sizes do not match in DataZone!");
        gtWarning() << params.size() << "params vs." << vals.size()
                    << "values vs." << units.size() << "units";
        clearData();
        return;
    }

    base()->m_params = params;
    base()->m_units = units;

    auto* tab = base()->table();
    if (!tab)
    {
        tab = createTable(*base());
    }

    tab->clear();

    if (std::any_of(std::begin(vals), std::end(vals), [&params](auto& vec) {
            return vec.size() != params.size();
        })) {
        gtWarning() << QObject::tr("Value sizes do not match in DataZone!");
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

    base()->table()->addAxis(axisName1, {}, "[-]", GtTableAxis::E_LINEAR,
                             GtTableAxis::I_LINEAR, GtTableAxis::E_LINEAR,
                             ticks);

    for (int i = 0; i < params.size(); i++)
    {
        const auto& values = newVals.at(i);

        if (!isValid(ticks, values))
        {
            gtWarning() << base()->tr("Ticks' size do not match values' size!");
            clearData();
            return;
        }

        tab->addValues(params.at(i), {}, units.at(i), values);
    }

    isValid();
}

void
GtDataZoneData::setData1Dfrom2DDataZone(GtDataZone* dataZone2D,
                                        int fixedAxisNumber,
                                        int fixedAxisTick)
{
    Q_ASSERT(m_base != nullptr);

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

    auto data = dataZone2D->fetchData();
    const auto& params = data.params();
    const auto& units = data.units();

    QVector<double> ticks;
    QString axisName;

    double fixedAxisValue;
    if (fixedAxisNumber == 0)
    {
        axisName = dataZone2D->axisNames().at(1);
        ticks = dataZone2D->allAxisTicks().at(1);
        auto otherTicks = base()->allAxisTicks().at(0);

        if (fixedAxisTick >= otherTicks.size())
        {
            return;
        }

        fixedAxisValue = otherTicks.at(fixedAxisTick);
    }
    else if (fixedAxisNumber == 1)
    {
        axisName = dataZone2D->axisNames().at(0);
        ticks = dataZone2D->allAxisTicks().at(0);
        auto otherTicks = base()->allAxisTicks().at(1);

        if (fixedAxisTick >= otherTicks.size())
        {
            return;
        }

        fixedAxisValue = otherTicks.at(fixedAxisTick);
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
                value = data.value2D(param, fixedAxisValue, ticks.at(k));
            }
            else if (fixedAxisNumber == 1)
            {
                value = data.value2D(param, ticks.at(k), fixedAxisValue);
            }
            currentVals.append(value);
        }

        vals.insert(param, currentVals);
    }

    setData1D(params, ticks, axisName, vals, units);
}


void
GtDataZoneData::setData2D(const QStringList& params,
                          const QVector<double>& ticks1,
                          const QVector<double>& ticks2,
                          const QString& axisName1,
                          const QString& axisName2,
                          const QMap<QString, QVector<double> >& vals,
                          const QStringList& units)
{
    Q_ASSERT(m_base != nullptr);

    if (params.isEmpty() || params.size() != vals.size() ||
            params.size() != units.size())
    {
        gtWarning() << QObject::tr("Parameter sizes do not match in DataZone!");
        gtWarning() << params.size() << "params vs." << vals.size()
                    << "values vs." << units.size() << "units";
        clearData();
        return;
    }

    base()->m_params = params;
    base()->m_units = units;

    auto* tab = base()->table();
    if (!tab)
    {
        tab = createTable(*base());
    }

    tab->clear();
    tab->addAxis(axisName1, {}, "[-]", GtTableAxis::E_LINEAR,
                 GtTableAxis::I_LINEAR, GtTableAxis::E_LINEAR,
                 ticks1);

    tab->addAxis(axisName2, {}, "[-]", GtTableAxis::E_LINEAR,
                 GtTableAxis::I_LINEAR, GtTableAxis::E_LINEAR,
                 ticks2);

    for (int i = 0; i < params.size(); i++)
    {
        const auto& param = params.at(i);
        auto pVals = vals.value(param);

        if (!isValid(ticks1, ticks2, pVals))
        {
            gtWarning() << QObject::tr("Ticks' size do not match values' size "
                                       "in DataZone!");
            clearData();
            return;
        }

        tab->addValues(param, {}, units.at(i), pVals);
    }

    isValid();
}

void
GtDataZoneData::clearData()
{
    Q_ASSERT(m_base != nullptr);

    auto* tab = base()->table();
    if (tab)
    {
        tab->clear();
    }

    base()->m_units.clear();
    base()->m_params.clear();
}

bool
GtDataZoneData::isValid() const
{
    Q_ASSERT(m_base != nullptr);

    if (!GtAbstractDataZoneData::isValid())
    {
        return false;
    }

    auto* tab = base()->table();
    if (!tab)
    {
        gtWarning() << QObject::tr("Null table in DataZone!");
        return false;
    }

    if (base()->m_params.size() != tab->tabValsKeys().size())
    {
        gtWarning() << QObject::tr("Param does not match table in DataZone!");

        gtDebug() << QObject::tr("Size Params:") << base()->m_params.size();
        gtDebug() << QObject::tr("Size Keys:") << tab->tabValsKeys().size();
        return false;
    }

    return true;
}

bool
GtDataZoneData::isValid(const QVector<double>& ticks,
                        const QVector<double>& vals)
{
    if (ticks.size() != vals.size())
    {
        gtWarning() << QObject::tr("Tick-Sizes do not match in DataZone!");
        return false;
    }

    return true;
}

bool
GtDataZoneData::isValid(const QVector<double>& ticks1,
                        const QVector<double>& ticks2,
                        const QVector<double>& vals)
{
    if (ticks1.size() * ticks2.size() != vals.size())
    {
        gtWarning() << QObject::tr("Tick-Sizes do not match in DataZone!");
        return false;
    }

    return true;
}

const QString&
GtDataZone::description() const
{
    return m_description;
}

void
GtDataZone::setDescription(const QString& description)
{
    m_description = description;
}

QStringList
GtDataZone::tabValsKeys() const
{
    auto* tab = table();
    if (!tab) return {};

    return tab->tabValsKeys();
}

void
GtDataZone::axisTicks(const QString& id, QVector<double>& axTicks) const
{
    if (axisNames().contains(id))
    {
        table()->getAxisTicks(id, axTicks);
    }
    else
    {
        gtWarning().nospace() << tr("DataZone does not contain Axis '")
                              << id << "'";
    }
}

void
GtDataZone::axisTicks(const QString& id, QStringList& axTicks) const
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
        gtWarning().nospace() << tr("DataZone does not contain Axis '")
                              << id << "'";
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
        retval.append(id + QStringLiteral(" = ") + QString::number(val));
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

double
GtDataZone::pyValue1D(const QString& param, const double& x0, bool* ok)
{
    internalize(); // will keep data fetched until externalize is called
    return fetchData().value1D(param, x0, ok);
}

double
GtDataZone::pyValue2D(const QString& param, const double& x0,
                      const double& x1, bool* ok)
{
    internalize(); // will keep data fetched until externalize is called
    return fetchData().value2D(param, x0, x1, ok);
}

double
GtDataZone::pyValue3D(const QString& param, const double& x0,
                      const double& x1, const double& x2, bool* ok)
{
    internalize(); // will keep data fetched until externalize is called
    return fetchData().value3D(param, x0, x1, x2, ok);
}

double
GtDataZone::pyValue4D(const QString& param, const double& x0,
                      const double& x1, const double& x2,
                      const double& x3, bool* ok)
{
    internalize(); // will keep data fetched until externalize is called
    return fetchData().value4D(param, x0, x1, x2, x3, ok);
}

QStringList
GtDataZone::axisNames() const
{
    return table()->getAxesNames();
}


GtDataZoneData::GtDataZoneData(GtDataZone* base) :
    GtAbstractDataZoneData(base)
{ }

void
GtDataZoneData::addModuleName(const QString& suffix)
{
    Q_ASSERT(m_base != nullptr);

    if (suffix.isEmpty())
    {
        return;
    }

    base()->table()->setTabValsKeysSuffix(suffix + QStringLiteral("."));

    GtAbstractDataZoneData::addModuleName(suffix);
}

double
GtDataZoneData::minValue2D(const QString& paramName, bool* ok)
{
    Q_ASSERT(m_base != nullptr);

    return minMaxHelper2D(*this, *base(), paramName, OperatorType::Min, ok);
}

double
GtDataZoneData::maxValue2D(const QString& paramName, bool* ok)
{
    Q_ASSERT(m_base != nullptr);

    return minMaxHelper2D(*this, *base(), paramName, OperatorType::Max, ok);
}

double
GtDataZoneData::minValue1D(const QString& paramName, bool* ok)
{
    Q_ASSERT(m_base != nullptr);

    return minMaxHelper1D(*this, *base(), paramName, OperatorType::Min, ok);
}

double
GtDataZoneData::maxValue1D(const QString& paramName, bool* ok)
{
    Q_ASSERT(m_base != nullptr);

    return minMaxHelper1D(*this, *base(), paramName, OperatorType::Max, ok);
}
