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
}

bool
GtDataZone0D::doFetchData(QVariant& metaData, bool fetchInitialVersion)
{
#ifdef GT_H5
    GtH5Data<QString, QString, double> data;

    GtH5ExternalizeHelper h5Worker{*this};

    if (!h5Worker.read(data, metaData, fetchInitialVersion))
    {
        return false;
    }

    data.deserialize(m_params, m_units, m_values);
#endif
    return true;
}

bool
GtDataZone0D::doExternalizeData(QVariant& metaData)
{
#ifdef GT_H5
    // serialize the data
    GtH5Data<QString, QString, double> data{m_params, m_units, m_values};

    GtH5ExternalizeHelper h5Worker{*this};

    return h5Worker.write(data, metaData);
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
GtDataZone0DData::clearData()
{
    Q_ASSERT(m_base != nullptr);

    base()->doClearExternalizedData();
    base()->changed();
}

bool
GtDataZone0DData::appendData(const QString& name,
                             const QString& unit,
                             const double& val,
                             bool overwrite)
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

        Q_ASSERT(base()->m_params.size() == base()->m_units.size() && base()->m_params.size() == base()->m_values.size() );

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
GtDataZone0DData::setValues(const QVector<double>& values)
{
    Q_ASSERT(m_base != nullptr);
    base()->m_values = values;
    base()->changed();
}

bool
GtDataZone0DData::setData(const QStringList& paramNames,
                          const QVector<double>& values,
                          const QStringList& units)
{
    Q_ASSERT(m_base != nullptr);

    if (paramNames.size() != values.size())
    {
        gtWarning() << "Datazone 0D: Setting data failed, because parameters "
                       "(" << paramNames.size() << ")" << " are not of same "
                       "size as values (" << values.size() << ")";
        return false;
    }

    if (paramNames.size() != units.size())
    {
        gtWarning() << "Datazone 0D: Setting data failed, because parameters "
                       "(" << paramNames.size() << ")" << " are not of same "
                       "size as units (" << units.size() << ")";
        return false;
    }

    if (paramNames.isEmpty() || values.isEmpty() || units.isEmpty())
    {
        gtWarning() << "Datazone 0D: Setting data failed, because parameters, "
                       "values or units are empty";
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
GtDataZone0DData::setValue(const QString& paramName, const double& value)
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
                             bool overwrite)
{
    return GtDataZone0DData::appendData(paramName, "-", value, overwrite);
}

bool
GtDataZone0DData::appendData(const QList<QString>& paramNames,
                             const QVector<double>& values)
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
                             const QVector<double>& values)
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

bool
GtDataZone0DData::isValid() const
{
    Q_ASSERT(m_base != nullptr);

    if (!GtAbstractDataZoneData::isValid())
    {
        return false;
    }

    if (base()->m_params.size() != base()->m_values.size())
    {
        gtWarning() << QObject::tr("Param size does not match value size!");
        return false;
    }

    return true;
}
