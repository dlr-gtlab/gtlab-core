/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 21.2.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#include "test_dmi_externalobject.h"

#include "gt_h5externalizehelper.h"
#include "genh5.h"

TestDmiExternalObjectData::TestDmiExternalObjectData(TestDmiExternalObject* base) :
    GtExternalizedObjectData(base)
{

}

const QStringList&
TestDmiExternalObjectData::params() const
{
    assert(m_base);
    return base()->m_params;
}

const QVector<double>&
TestDmiExternalObjectData::values() const
{
    assert(m_base);
    return base()->m_values;
}

double
TestDmiExternalObjectData::value(int idx) const
{
    assert(m_base);
    if (idx < 0 || idx > base()->m_values.size())
    {
        return std::numeric_limits<double>::min();
    }
    return base()->m_values[idx];
}

const QString&
TestDmiExternalObjectData::param(int idx) const
{
    assert(m_base);
    if (idx < 0 || idx > base()->m_params.size())
    {
        static const QString s;
        return s;
    }
    return base()->m_params[idx];
}

void
TestDmiExternalObjectData::setParams(QStringList params) const
{
    assert(m_base);
    base()->m_params = std::move(params);
    base()->changed();
}

void
TestDmiExternalObjectData::setValues(QVector<double> vals) const
{
    assert(m_base);
    base()->m_values = std::move(vals);
    base()->changed();
}

TestDmiExternalObject::TestDmiExternalObject() :
    m_params{"param_0", "param_1", "param_2"},
    m_values{0.123, 1.234, 2.345}
{
    setObjectName("External_Data");
    setFlag(UserDeletable);
    setFlag(UserRenamable);
}

bool
TestDmiExternalObject::isDataValid() const
{
    return m_params.size() == m_values.size();
}

bool
TestDmiExternalObject::canExternalize() const
{
#ifndef GT_H5
    return false;
#else
    gtDebug() << __FUNCTION__ << isDataValid() << m_params.size() << m_values.size() << refCount();
    return isDataValid() && !m_params.empty();
#endif
}

bool
TestDmiExternalObject::doFetchData(QVariant& metaData, bool fetchInitialVersion)
{
    try
    {
        gtDebug() << __FUNCTION__ << isDataValid() << m_params.size() << m_values.size() << refCount() << fetchInitialVersion;
        GenH5::Data<QString, double> data;
        data.setTypeNames({"params", "values"});

        GtH5ExternalizeHelper helper{*this};

        if (!helper.read(data, metaData, fetchInitialVersion))
        {
            return false;
        }

        data.unpack(m_params, m_values);
        return true;
    }
    catch (GenH5::Exception const& e)
    {
        gtError() << e.what();
    }
    return false;
}

bool
TestDmiExternalObject::doExternalizeData(QVariant& metaData)
{
    gtDebug() << __FUNCTION__ << isDataValid() << m_params.size() << m_values.size() << refCount();
    try
    {
        GenH5::Data<QString, double> data{m_params, m_values};
        data.setTypeNames({"params", "values"});

        GtH5ExternalizeHelper helper{*this};

        return helper.write(data, metaData);
    }
    catch (GenH5::Exception const& e)
    {
        gtError() << e.what();
    }
    return false;
}

void
TestDmiExternalObject::doClearExternalizedData()
{
    gtDebug() << __FUNCTION__ << isDataValid() << m_params.size() << m_values.size() << refCount();
    m_params.clear();
    m_values.clear();
}

void
TestDmiExternalObject::onObjectDiffMerged()
{
    emit dataChanged(this);
    return GtExternalizedObject::onObjectDiffMerged();
}
