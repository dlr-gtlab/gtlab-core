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

#include <array>

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
    double minMaxDouble(OperatorType op)
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
    auto minMaxOperator(OperatorType op)
    {
        return op == OperatorType::Max ? maxFunction : minFunction;
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

    /// helper method for accessing value of nDim
    template <size_t nDim, typename Lambda>
    double valueHelper(const GtDataZone& dz,
                       const GtTable* table,
                       const QString& param,
                       bool* ok,
                       Lambda func)
    {
        if (dz.nDims() != nDim)
        {
            gtWarning() << QObject::tr("Trying to get a")
                        << QString::number(nDim) +
                           QObject::tr("D value out of a")
                        << QString::number(dz.nDims()) +
                           QObject::tr("D Table!");
            return error(0.0, ok);
        }

        if (!table || param.isEmpty() || !table->tabValsKeys().contains(param))
        {
            return error(0.0, ok);
        }

        try
        {
            return success(func(*table), ok);
        }
        catch (GTlabException& /*e*/)
        {
            return error(0.0, ok);
        }
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
    try
    {
        auto* table = this->table();

        // retrieve rows and cols of table
        int rows = m_params.size();
        int cols = GenH5::prod(table->dimensions());

        // retrieve table values
        auto tabVals = table->findDirectChildren<GtTableValues*>();

        GenH5::Data<double> data;

        GtH5ExternalizeHelper h5Worker{*this};

        // read the data
        if (!h5Worker.read(data, metaData, fetchInitialVersion))
        {
            return false;
        }

        if (data.size() != rows * cols)
        {
            gtError() << tr("HDF5: Could not read from the dataset!")
                      << tr("(invalid table values dimension)");
            return false;
        }

        // deserialize the data
        for (int row = 0; row < rows; ++row)
        {
            auto* tabVal = tabVals.at(row);

            // table values may not be fully cleared (e.g. due to diff) we
            // have to make sure to only restore the missing/empty table values
            if (tabVal->values().size() != cols)
            {
                // splitt 1D values vector into 2D value vectors of size 'cols'
                tabVal->setValues(data.mid(row * cols, cols));
            }
        }
    }
    catch (GenH5::Exception const& e)
    {
        gtWarning() << "Exception:" << e.what();
        return false;
    }
    catch (H5::Exception& /*e*/)
    {
        gtWarning() << "HDF5 Exception:";
        return false;
    }
#endif
    return true;
}

bool
GtDataZone::doExternalizeData(QVariant& metaData)
{
#ifdef GT_H5
    try
    {
        auto* table = this->table();

        // retrieve rows and cols of table
        int rows{m_params.size()};
        int cols{GenH5::prod(table->dimensions())};

        // serialize the data
        GenH5::Data<double> data;
        data.reserve(rows * cols);

        for (const auto* tabVal : table->findDirectChildren<GtTableValues*>())
        {
            data.append(tabVal->values());
        }

        if (data.size() != rows * cols)
        {
            gtError() << tr("HDF5: Could not write to the dataset!")
                      << tr("(invalid table values dimension)");
            return false;
        }

        GtH5ExternalizeHelper h5Worker{*this};

        // write the data
        return h5Worker.write(data, metaData);
    }
    catch (GenH5::Exception const& e)
    {
        gtWarning() << "Exception:" << e.what();
        return false;
    }
    catch (H5::Exception& /*e*/)
    {
        gtWarning() << "HDF5 Exception:";
        return false;
    }
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

bool
GtDataZone::canExternalize() const
{
    // can only externalize if data is valid and not empty
    return isDataValid() && !m_params.empty();
}

bool
GtDataZone::isDataValid() const
{
    auto* tab = table();
    if (!tab)
    {
        gtWarning() << tr("Invalid DataZone!") << tr("(Null table)");
        return false;
    }

    auto size = m_params.size();
    if (size != tab->tabValsKeys().size() ||
        size != tab->findDirectChildren<GtTableValues*>().size())
    {
        gtWarning() << tr("Invalid DataZone!")
                    << tr("(Parameter size does not match units size)");
        gtDebug() << tr("Size Params:") << m_params.size();
        gtDebug() << tr("Size Keys:  ") << tab->tabValsKeys().size();
        gtDebug() << tr("Size Values:")
                  << tab->findDirectChildren<GtTableValues*>().size();
        return false;
    }
    return true;
}

void
GtDataZone::addModuleName(const QString& moduleName)
{
    if (moduleName.isEmpty() || !table())
    {
        return;
    }

    table()->setTabValsKeysSuffix(moduleName + QStringLiteral("."));

    GtAbstractDataZone::addModuleName(moduleName);
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

QVector<double>
GtDataZoneData::value1DVector(const QString& param, bool* ok) const
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

        return valueAndSetError(retVal, success, ok);
    }

    return error(QVector<double>(), ok);
}

double
GtDataZoneData::value1D(const QString& param, double x0, bool* ok) const
{
    Q_ASSERT(m_base != nullptr);

    return valueHelper<1>(*base(), base()->table(), param, ok,
                          [&](const GtTable& table) {
        return table.getValue1D(param, x0);
    });
}

double
GtDataZoneData::value2D(const QString& param, double x0, double x1,
                        bool* ok) const
{
    Q_ASSERT(m_base != nullptr);

    return valueHelper<2>(*base(), base()->table(), param, ok,
                          [&](const GtTable& table) {
        return table.getValue2D(param, x0, x1);
    });
}

double
GtDataZoneData::value3D(const QString& param, double x0, double x1, double x2,
                        bool* ok) const
{
    Q_ASSERT(m_base != nullptr);

    return valueHelper<3>(*base(), base()->table(), param, ok,
                          [&](const GtTable& table) {
        return table.getValue3D(param, x0, x1, x2);
    });
}

double
GtDataZoneData::value4D(const QString& param, double x0, double x1, double x2,
                        double x3, bool* ok) const
{
    Q_ASSERT(m_base != nullptr);

    return valueHelper<4>(*base(), base()->table(), param, ok,
                          [&](const GtTable& table) {
        return table.getValue4D(param, x0, x1, x2, x3);
    });
}

bool
GtDataZoneData::setData(const QStringList& params,
                        const QMap<QString, QVector<double> >& axisTicks,
                        const QMap<QString, QVector<double> >& paramValues,
                        const QStringList& units) &
{
    Q_ASSERT(m_base != nullptr);

    static auto errMsg = QObject::tr("Setting DataZone Data failed!");

    // basic bounds checking
    auto size = params.size();
    if (params.isEmpty() || size != units.size() || size != paramValues.size())
    {
        gtError() << errMsg
                  << QObject::tr("(Parameter sizes differ or are empty)");
        gtWarning() << size << "parameters vs."
                    << paramValues.size() << "values vs."
                    <<  units.size() << "units";
        clearData();
        return false;
    }

    // check if all values have the same size
    auto valuesSize = paramValues.first().size();
    bool equalValueSizes = std::all_of(std::cbegin(paramValues),
                                       std::cend(paramValues),
                                       [&](auto& vec) {
        return vec.size() == valuesSize;
    });
    if (valuesSize == 0 || !equalValueSizes)
    {
        gtError() << errMsg << QObject::tr("(Value sizes differ)");
        clearData();
        return false;
    }

    // check if product of axis ticks equals value size
    auto totalTicks = std::accumulate(std::cbegin(axisTicks),
                                      std::cend(axisTicks),
                                      int{1}, [](auto sum, auto& vec) {
        return sum * vec.size();
    });
    if (axisTicks.isEmpty() || totalTicks != valuesSize)
    {
        gtError() << errMsg
                  << QObject::tr("(Axis ticks do not match value size!)");
        gtWarning() << totalTicks << "total ticks vs."
                    << valuesSize << "values per parameter";
        clearData();
        return false;
    }

    // assign params and units
    base()->m_params = params;
    base()->m_units = units;

    // setup table
    auto* tab = base()->table();
    if (!tab)
    {
        tab = createTable(*base());
    }
    tab->clear();

    // create axis
    bool success = true;
    gt::for_each_key (axisTicks, [&](const QString& axisName) {
        success &= tab->addAxis(axisName, {}, "[-]", GtTableAxis::E_LINEAR,
                                GtTableAxis::I_LINEAR, GtTableAxis::E_LINEAR,
                                axisTicks.value(axisName));
    });
    if (!success)
    {
        clearData();
        return false;
    }

    // set values
    for (int i = 0; i < size; ++i)
    {
        auto& param = params.at(i);
        auto& values = paramValues.value(param, {});
        if (values.isEmpty())
        {
            gtError() << errMsg << QObject::tr("(Parameter '") + param +
                         QObject::tr("' was not found in values!)");
            clearData();
            return false;
        }
        if (!tab->addValues(param, {}, units.at(i), values))
        {
            clearData();
            return false;
        }
    }

    return isValid();
}

bool
GtDataZoneData::setData1D(const QStringList& params,
                          const QVector<double>& ticks,
                          const QString& axisName,
                          const QMap< QString, QVector<double> >& vals,
                          const QStringList& units) &
{
    Q_ASSERT(m_base != nullptr);

    QMap< QString, QVector<double> > axis;
    axis.insert(axisName, ticks);
    return setData(params, axis, vals, units);
}

bool
GtDataZoneData::setData1D(const QStringList& params,
                          const QMap<double, QVector<double> >& vals,
                          const QString& axisName,
                          const QStringList& units) &
{
    Q_ASSERT(m_base != nullptr);

    auto size = params.size();
    auto equalValueSizes = std::all_of(std::cbegin(vals),
                                       std::cend(vals),
                                       [&](auto& vec) {
        return vec.size() == size;
    });
    if (vals.isEmpty() || !equalValueSizes)
    {
        gtError() << QObject::tr("Setting DataZone 1D Data failed!")
                  << QObject::tr("(Value sizes do not match in DataZone!)");
        return false;
    }

    QVector<double> ticks;
    QVector< QVector<double> > valueVecs;
    valueVecs.resize(params.size());
    for (auto& vec : valueVecs)
    {
        vec.reserve(size);
    }

    // loop through vals to sort them to
    //           t1,   t2,   t3
    // param1: val1, val2, val3
    // param2: val1, val2, val3
    // param3: val1, val2, val3
    gt::for_each_key (vals, [&](double key)
    {
        ticks.append(key);
        for (int i = 0; i < size; ++i)
        {
            valueVecs[i].append(vals.value(key).at(i));
        }
    });

    QMap< QString, QVector<double> > axis;
    axis.insert(axisName, ticks);

    QMap< QString, QVector<double> > valueMap;
    for (int i = 0; i < size; ++i)
    {
        valueMap.insert(params.at(i), valueVecs.at(i));
    }

    return setData(params, axis, valueMap, units);
}

bool
GtDataZoneData::addDataPoint1D(const QMap<QString, double>& vals,
                               const double& tick,
                               bool overwrite) &
{
    Q_ASSERT(m_base != nullptr);

    /// Function only valid for 1-D Datazones
    if (base()->nDims() != 1)
    {
        gtError() << QObject::tr("Cannot add 1D-values to ") << base()->nDims()
                  << QObject::tr("-D datazone");
        return false;
    }

    QStringList params = vals.keys();

    if (base()->m_params != params)
    {
        gtError() << QObject::tr("The map does not fit "
                                 "to the given parameters.");
        gtWarning() << QObject::tr("Given parameters are ") << base()->m_params;
        gtWarning() << QObject::tr("Try to set the parameters") << params;
        return false;
    }

    QVector<double> axisTicks = base()->axisTicks();

    if (axisTicks.isEmpty())
    {
        gtError() << QObject::tr("Invalid axis of the datzone");
        return false;
    }

    /// option to handle existing tick entry
    if (axisTicks.contains(tick))
    {
        if (!overwrite)
        {
            gtError() << QObject::tr("Try to overwrite value which is "
                                     "explicitly not allowed");
            return false;
        }

        int i = axisTicks.indexOf(tick);

        for (const QString& p : params)
        {
            if (auto v = base()->table()->getVals(p))
            {
                v->replaceValue(i, vals[p]);
            }
        }

        return true;
    }

    int smallerIndex = -1;

    for (int i = 0; i < axisTicks.size(); ++i)
    {
        if (axisTicks[i] > tick)
        {
            break;
        }
        smallerIndex = i;
    }

    /// first index
    if (smallerIndex == -1)
    {
        for (const QString& p : params)
        {
            if (auto v = base()->table()->getVals(p))
            {
                v->prependValue(vals[p]);
            }
        }

        auto ax = base()->table()->getAxesList().first();
        ax->prependTick(tick);

        return true;
    }

    /// last index
    if (smallerIndex == axisTicks.size() - 1)
    {
        for (const QString& p : params)
        {
            if (auto v = base()->table()->getVals(p))
            {
                v->appendValue(vals[p]);
            }
        }

        auto ax = base()->table()->getAxesList().first();
        ax->appendTick(tick);
        return true;
    }

    for (const QString& p : params)
    {
        if (auto v = base()->table()->getVals(p))
        {
            v->insertValue(smallerIndex + 1, vals[p]);
        }
    }

    auto ax = base()->table()->getAxesList().first();
    ax->insertTick(smallerIndex + 1, tick);

    return true;
}

bool
GtDataZoneData::setData1Dfrom2DDataZone(GtDataZone* dataZone2D,
                                        int fixedAxisIdx,
                                        int fixedAxisTick) &
{
    Q_ASSERT(m_base != nullptr);

    static auto errMsg = QObject::tr("Setting 1-D DataZone from 2-D DataZone "
                                     "failed!");

    if (!dataZone2D)
    {
        gtError() << errMsg << QObject::tr("(Invalid DataZone)");
        return false;
    }

    if (dataZone2D->nDims() != 2)
    {
        gtError() << errMsg << QObject::tr("(Not a 2D DataZone)");
        return false;
    }
    if (!(fixedAxisIdx == 0 || fixedAxisIdx == 1))
    {
        gtError() << errMsg << QObject::tr("(Invalid Axis selected)");
        return false;
    }

    // is always the oppsoite of fixedAxisIdx (either 0 or 1)
    auto otherAxisIdx = 1 - fixedAxisIdx;

    auto data = dataZone2D->fetchData();
    if (!data.isValid())
    {
        gtError() << errMsg << QObject::tr("(Invalid DataZone Data)");
        return false;
    }
    const auto& params = data.params();
    const auto& units = data.units();

    auto ticks = dataZone2D->allAxisTicks().at(otherAxisIdx);
    auto axisName = dataZone2D->axisNames().at(otherAxisIdx);
    auto otherTicks = base()->allAxisTicks().at(fixedAxisIdx);

    if (fixedAxisTick >= otherTicks.size())
    {
        return false;
    }

    double fixedAxisValue = otherTicks.at(fixedAxisTick);

    QMap< QString, QVector<double> > values;
    bool ok = true;
    auto size = params.size();

    for (const QString& param : qAsConst(params))
    {
        QVector<double> currentVals;
        currentVals.reserve(size);
        for (double tick : qAsConst(ticks))
        {
            // helper array to access correct coors
            const std::array<double, 2> coors{ fixedAxisValue, tick };
            double ax1 = coors[static_cast<size_t>(fixedAxisIdx)];
            double ax2 = coors[static_cast<size_t>(otherAxisIdx)];

            currentVals.append(data.value2D(param, ax1, ax2, &ok));

            if (!ok)
            {
                return false;
            }
        }

        values.insert(param, currentVals);
    }

    QMap< QString, QVector<double> > axis;
    axis.insert(axisName, ticks);
    return setData(params, axis, values, units);
}


bool
GtDataZoneData::setData2D(const QStringList& params,
                          const QVector<double>& ticks1,
                          const QVector<double>& ticks2,
                          const QString& axisName1,
                          const QString& axisName2,
                          const QMap<QString, QVector<double> >& vals,
                          const QStringList& units) &
{
    Q_ASSERT(m_base != nullptr);

    QMap< QString, QVector<double> > axis;
    axis.insert(axisName1, ticks1);
    axis.insert(axisName2, ticks2);
    return setData(params, axis, vals, units);
}

void
GtDataZoneData::clearData() &
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
        gtWarning() << tr("DataZone does not contain Axis ")
                    << "'" + id + "'";
    }
}

void
GtDataZone::axisTicks(const QString& id, QStringList& axTicks) const
{    
    QVector<double> ticks;
    axisTicks(id, ticks);
    axTicks.clear();
    axTicks.reserve(ticks.size());

    std::transform(std::cbegin(ticks), std::cend(ticks),
                   std::back_inserter(axTicks), [](auto tick){
        return QString::number(tick);
    });
}

QVector<double>
GtDataZone::axisTicks(const QString& id) const
{
    QVector<double> retVal;

    axisTicks(id, retVal);

    return retVal;
}

QVector<double>
GtDataZone::axisTicks(int axisIndex) const
{
    QVector<double> retVal;

    QStringList axes = axisNames();

    if (axes.size() <= axisIndex)
    {
        gtWarning().nospace() << tr("DataZone does not contain Axis of index")
                              << "'" << axisIndex << "'";
        return retVal;
    }

    axisTicks(axes[axisIndex], retVal);

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
