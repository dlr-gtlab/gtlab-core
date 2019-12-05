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

GtDataZone0D::GtDataZone0D()
{
    setFlag(UserDeletable);
}

bool
GtDataZone0D::appendData(const QString& name, const QString& unit, double val)
{
    if (name.isEmpty())
    {
        gtWarning() << tr("could not append data!") << QStringLiteral(" ")
                    << tr("empty parameter name");

        return false;
    }

    if (m_params.contains(name))
    {
        gtWarning() << tr("could not append data!") << QStringLiteral(" ")
                    << tr("parameter name already exists");

        return false;
    }

    m_params.append(name);
    m_units.append(unit);
    m_values.append(val);

    isValid();

    changed();

    return true;
}

bool
GtDataZone0D::is0D()
{
    return true;
}

int
GtDataZone0D::nDims()
{
    return 0;
}

void
GtDataZone0D::addModuleName(QString moduleName)
{
    if (moduleName == "")
    {
        return;
    }

    QStringList newParamNames;

    foreach (QString param, params())
    {
        newParamNames.append(moduleName + "." + param);
    }

    m_params = newParamNames;
}

QVector<double>
GtDataZone0D::values() const
{
    return m_values;
}

void
GtDataZone0D::setValues(const QVector<double>& values)
{
    m_values.clear();
    m_values = values;
    changed();
}

QString
GtDataZone0D::unit(QString param)
{
    QString retval = QString();

    int index = m_params.indexOf(param);

    if (index == -1)
    {
        gtDebug() << tr("Param ") << param
                  << tr(" could not be found in 0D data, "
                        "no unit can be shown");
        return retval;
    }

    retval = m_units.at(index);

    return retval;
}

bool
GtDataZone0D::setData(QStringList paramNames,
                      const QVector<double>& values,
                      QStringList units)
{
    clearData();

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

    m_params = paramNames;

    m_values = values;

    m_units = units;

    isValid();

    changed();

    return true;
}

// TODO check if unit of paramName is the same as the one in m_units(paramName)
bool
GtDataZone0D::setValue(const QString& paramName, const double& value)
{
    int index = m_params.indexOf(paramName);

    if (index == -1 || m_params.size() != m_values.size())
    {
        return false;
    }

    m_values[index] = value;

    changed();

    return true;
}

bool
GtDataZone0D::appendData(QString paramName, double value)
{
    if (paramName.isEmpty())
    {
        return false;
    }

    if (m_params.contains(paramName) || m_params.size() != m_values.size())
    {
        return false;
    }

    m_params.append(paramName);
    m_values.append(value);

    changed();

    return true;
}

bool
GtDataZone0D::appendData(const QList<QString>& paramNames,
                         const QVector<double>& values)
{
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

    changed();

    return true;
}

double
GtDataZone0D::value(QString paramName, bool* ok)
{
    int index = m_params.indexOf(paramName);

    if (index == -1 || m_params.size() != m_values.size())
    {
        if (ok)
        {
            *ok = false;
        }

//        gtWarning() << tr("Value of Parameter '") << paramName
//                  << tr("'not found in Datazone, returning 0.0");

        return 0.0;
    }

    return m_values.at(index);
}


void
GtDataZone0D::clearData()
{
    m_params.clear();
    m_values.clear();
    m_units.clear();

    changed();
}

bool
GtDataZone0D::isValid()
{
    if (m_params.size() != m_values.size())
    {
        gtWarning() << tr("Param size does not match value size!");
        return false;
    }

    if (m_params.size() != m_units.size())
    {
        gtWarning() << tr("Param size does not match units size!");
        return false;
    }

    return true;
}

