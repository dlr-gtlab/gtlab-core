/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 08.12.2015
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */

#include "gt_abstractdatazone.h"
#include "gt_logging.h"

GtAbstractDataZone::GtAbstractDataZone() = default;

GtAbstractDataZoneData::GtAbstractDataZoneData(GtAbstractDataZone* base) :
    GtExternalizedObjectData{base}
{ }

QString
GtAbstractDataZoneData::unit(const QString& param) const
{
    Q_ASSERT(m_base != nullptr);

    int index = base()->m_params.indexOf(param);

    if (index == -1)
    {
        gtDebug() << QObject::tr("Param") << param
                  << QObject::tr("could not be found in datazone, "
                                 "no unit can be shown");
        return {};
    }

    return base()->m_units.at(index);
}

void
GtAbstractDataZoneData::addModuleName(const QString& moduleName) &
{
    Q_ASSERT(m_base != nullptr);

    base()->addModuleName(moduleName);
}

const QStringList&
GtAbstractDataZoneData::params() const
{
    Q_ASSERT(m_base != nullptr);
    return base()->m_params;
}

void
GtAbstractDataZoneData::setParams(const QStringList& params) &
{
    Q_ASSERT(m_base != nullptr);
    base()->m_params = params;
    base()->changed();
}

const QStringList&
GtAbstractDataZoneData::units() const
{
    Q_ASSERT(m_base != nullptr);
    return base()->m_units;
}

void
GtAbstractDataZoneData::setUnits(const QStringList& units) &
{
    Q_ASSERT(m_base != nullptr);
    base()->m_units = units;
    base()->changed();
}

bool
GtAbstractDataZoneData::hasParam(const QString& param) const
{
    Q_ASSERT(m_base != nullptr);
    return base()->m_params.contains(param);
}

bool
GtAbstractDataZone::isDataValid() const
{
    if (m_params.size() != m_units.size())
    {
        gtWarning() << tr("Invalid DataZone!")
                    << tr("(Parameter size does not match units size)");
        return false;
    }
    return GtExternalizedObject::isDataValid();
}

void
GtAbstractDataZone::addModuleName(const QString& moduleName)
{
    if (moduleName.isEmpty())
    {
        return;
    }

    for (QString& param : m_params)
    {
        param.prepend(moduleName + QStringLiteral("."));
    }
}
