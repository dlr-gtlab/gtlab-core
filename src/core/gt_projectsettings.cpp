/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 *
 *  Author: Martin Siggel (AT-TWK)
 */

#include "gt_projectsettings.h"

#include <gt_qtutilities.h>

#include <QJsonObject>


constexpr const char* KEY_IGNORE_IRREGULARITIES = "IgnoreIrregularities";
constexpr const char* KEY_ALLOW_OBJECTSERIALIZE = "AllowOwnObjectSerialization";

GtProjectSettings::GtProjectSettings()
{
    initDefaultValues();
}

bool
GtProjectSettings::ignoringIrregularities() const
{
    return value(KEY_IGNORE_IRREGULARITIES, false).toBool();
}

void
GtProjectSettings::setIgnoreIrregularities(bool ignore)
{
    setValue(KEY_IGNORE_IRREGULARITIES, ignore);
}

bool
GtProjectSettings::ownObjectFileSerializationEnabled() const
{
    return value(KEY_ALLOW_OBJECTSERIALIZE, true).toBool();
}

void
GtProjectSettings::setOwnObjectFileSerializationEnabled(bool enable)
{
    setValue(KEY_ALLOW_OBJECTSERIALIZE, enable);
}

void
GtProjectSettings::setValue(const QString& key, const QVariant& value)
{
    auto iter = m_settings.find(key);

    if (iter != m_settings.end())
    {
        if (gt::qvariantCompare(value, *iter) != 0)
        {
            *iter = value;
            emit changed();
        }
    }
    else
    {
        m_settings.insert(key, value);
        emit changed();
    }
}

QVariant&
GtProjectSettings::operator[](const QString& key)
{
    return m_settings[key];
}

QVariant
GtProjectSettings::value(const QString& key, const QVariant& defaultValue) const
{
    auto it = m_settings.constFind(key);
    return (it != m_settings.cend()) ? *it : defaultValue;
}

bool
GtProjectSettings::contains(const QString& key) const
{
    return m_settings.contains(key);
}

int
GtProjectSettings::remove(const QString& key)
{
    return m_settings.remove(key);
}

void
GtProjectSettings::clear()
{
    m_settings.clear();
}

int
GtProjectSettings::size() const
{
    return m_settings.size();
}

bool
GtProjectSettings::isEmpty() const
{
    return m_settings.isEmpty();
}

QJsonValue
GtProjectSettings::toJson() const
{
    QJsonObject obj;
    for (auto it = m_settings.cbegin(); it != m_settings.cend(); ++it)
    {
        obj.insert(it.key(), QJsonValue::fromVariant(it.value()));
    }
    return QJsonValue(obj);
}

void
GtProjectSettings::fromJson(const QJsonValue& json)
{
    initDefaultValues();

    if (!json.isObject())
        return; // silently ignore non-object input, or you could add error handling

    const QJsonObject obj = json.toObject();
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it)
    {
        m_settings.insert(it.key(), it.value().toVariant());
    }
}

void
GtProjectSettings::initDefaultValues()
{
    m_settings.clear();

    m_settings[KEY_IGNORE_IRREGULARITIES] = false;
    m_settings[KEY_ALLOW_OBJECTSERIALIZE] = true;
}

GtProjectSettings::const_iterator
GtProjectSettings::cbegin() const
{
    return m_settings.cbegin();
}

GtProjectSettings::const_iterator
GtProjectSettings::end() const
{
    return m_settings.end();
}

GtProjectSettings::const_iterator
GtProjectSettings::begin() const
{
    return m_settings.begin();
}

GtProjectSettings::iterator
GtProjectSettings::end()
{
    return m_settings.end();
}

GtProjectSettings::iterator
GtProjectSettings::begin()
{
    return m_settings.begin();
}
