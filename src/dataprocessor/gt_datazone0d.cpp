/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include "gt_datazone0d.h"
#include "gt_logging.h"
#include "gt_h5externalizehelper.h"

GtDataZone0D::GtDataZone0D()
{
    setFlag(UserDeletable);
    connect(this, SIGNAL(valuesChanged()), this, SLOT(changed()));
}

bool
GtDataZone0D::doFetchData(QVariant& metaData, bool fetchInitialVersion)
{
#ifdef GT_H5
    // See issue #249
    if (m_params.size() == 1 && m_params.first().isEmpty()) m_params.clear();
    if (m_units.size()  == 1 && m_units.first().isEmpty())  m_units.clear();

    // datazone may not be fully cleared (e.g. due to diff)
    // we have to make sure to only restore the missing/empty properties
    auto paramsDup = m_params;
    auto unitsDup  = m_units;
    auto valuesDup = m_values;

    try
    {
        GenH5::CompData<QString, QString, double> data{};

        GtH5ExternalizeHelper h5Worker{*this};

        if (!h5Worker.read(data, metaData, fetchInitialVersion))
        {
            return false;
        }

        // deserialize will not clear params, units and values
        doClearExternalizedData();
        data.deserialize(m_params, m_units, m_values);
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

    // merge data
    if (m_params.size() == paramsDup.size()) m_params = paramsDup;
    if (m_units.size()  == unitsDup.size())  m_units  = unitsDup;
    if (m_values.size() == valuesDup.size()) m_values = valuesDup;
#endif
    return true;
}

bool
GtDataZone0D::doExternalizeData(QVariant& metaData)
{
#ifdef GT_H5
    try
    {
        // serialize the data
        GenH5::CompData<QString, QString, double> data{
            m_params, m_units, m_values
        };
        // set compound type names
        data.setTypeNames({"params", "values", "units"});

        GtH5ExternalizeHelper h5Worker{*this};

        return h5Worker.write(data, metaData);
    }
    catch (GenH5::Exception const& e)
    {
        gtWarning() << "Exception:" << e.what();
        return false;
    }
    catch (H5::Exception const& /*e*/)
    {
        gtWarning() << "HDF5 Exception:";
        return false;
    }
#else
    return true;
#endif
}

void
GtDataZone0D::doClearExternalizedData()
{
#ifdef GT_H5
    m_params.clear();
    m_values.clear();
    m_units.clear();
#endif
}

bool
GtDataZone0D::canExternalize() const
{
    // can only externalize if data is valid and not empty
    return isDataValid() && !m_params.empty();
}

bool
GtDataZone0D::isDataValid() const
{
    if (m_params.size() != m_values.size())
    {
        gtWarning() << tr("Invalid DataZone!")
                    << tr("(Parameter size does not match value size)");
        return false;
    }
    return GtAbstractDataZone::isDataValid();
}

bool
GtDataZone0D::is0D() const
{
    return true;
}

int
GtDataZone0D::nDims() const
{
    return 0;
}

GtDataZone0DData::GtDataZone0DData(GtDataZone0D* base) :
    GtAbstractDataZoneData(base)
{

}

void
GtDataZone0DData::clearData() &
{
    Q_ASSERT(m_base != nullptr);

    base()->doClearExternalizedData();
    base()->changed();
}

bool
GtDataZone0DData::appendData(const QString& name,
                             const QString& unit,
                             const double& val,
                             bool overwrite) &
{
    Q_ASSERT(m_base != nullptr);

    if (name.isEmpty())
    {
        gtWarning() << QObject::tr("could not append data!")
                    << QObject::tr("empty parameter name");

        return false;
    }

    if (base()->m_params.contains(name))
    {
        if (!overwrite)
        {
            gtWarning() << QObject::tr("could not append data!")
                        << QObject::tr("parameter name already exists");

            return false;
        }

        int i = base()->m_params.indexOf(name);

        Q_ASSERT(base()->m_params.size() == base()->m_units.size() &&
                 base()->m_params.size() == base()->m_values.size() );

        base()->m_units[i] = unit;
        base()->m_values[i] = val;
    }
    else
    {
        base()->m_params.append(name);
        base()->m_units.append(unit);
        base()->m_values.append(val);
    }


    base()->changed();

    return isValid();
}

const QVector<double>&
GtDataZone0DData::values() const
{
    Q_ASSERT(m_base != nullptr);
    return base()->m_values;
}

void
GtDataZone0DData::setValues(const QVector<double>& values) &
{
    Q_ASSERT(m_base != nullptr);
    base()->m_values = values;
    base()->changed();
}

bool
GtDataZone0DData::setData(const QStringList& paramNames,
                          const QVector<double>& values,
                          const QStringList& units) &
{
    Q_ASSERT(m_base != nullptr);

    auto size = paramNames.size();
    if (paramNames.empty() || size != values.size() || size != units.size())
    {
        gtWarning() << QObject::tr("Setting Datazone0D Data failed!")
                    << QObject::tr("(Parameter sizes differ or are empty)");
        gtWarning() << size << "parameters vs."
                    << values.size() << "values vs."
                    <<  units.size() << "units";
        return false;
    }

    base()->m_params = paramNames;
    base()->m_values = values;
    base()->m_units = units;

    base()->changed();

    return isValid();
}

// TODO check if unit of paramName is the same as the one in m_units(paramName)
bool
GtDataZone0DData::setValue(const QString& paramName, const double& value) &
{
    Q_ASSERT(m_base != nullptr);

    int index = base()->m_params.indexOf(paramName);

    if (index == -1 || base()->m_params.size() != base()->m_values.size())
    {
        return false;
    }

    base()->m_values[index] = value;

    base()->changed();

    return true;
}

bool
GtDataZone0DData::appendData(const QString& paramName,
                             const double& value,
                             bool overwrite) &
{
    return GtDataZone0DData::appendData(paramName, "-", value, overwrite);
}

bool
GtDataZone0DData::appendData(const QList<QString>& paramNames,
                             const QVector<double>& values) &
{
    Q_ASSERT(m_base != nullptr);

    if (paramNames.isEmpty() || values.isEmpty())
    {
        return false;
    }

    if (paramNames.size() != values.size())
    {
        return false;
    }

    for (int i = 0; i < paramNames.size(); i++)
    {
        if (!appendData(paramNames[i], values[i]))
        {
            return false;
        }
    }

    base()->changed();

    return true;
}

bool
GtDataZone0DData::appendData(const QList<QString>& paramNames,
                             const QList<QString>& units,
                             const QVector<double>& values) &
{
    Q_ASSERT(m_base != nullptr);

    if (paramNames.isEmpty() || values.isEmpty() || units.isEmpty())
    {
        return false;
    }

    if (paramNames.size() != values.size() || paramNames.size() != units.size())
    {
        return false;
    }

    for (int i = 0; i < paramNames.size(); i++)
    {
        if (!appendData(paramNames[i], units[i], values[i]))
        {
            return false;
        }
    }

    base()->changed();

    return true;
}

double
GtDataZone0DData::value(const QString& paramName, bool* ok) const
{
    Q_ASSERT(m_base != nullptr);

    if (ok != nullptr)
    {
        *ok = false;
    }

    int index = base()->m_params.indexOf(paramName);

    if (index == -1 || base()->m_params.size() != base()->m_values.size())
    {
        return 0.0;
    }

    if (ok != nullptr)
    {
        *ok = true;
    }
    return base()->m_values.at(index);
}
