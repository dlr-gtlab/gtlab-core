/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 30.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QSettings>

#include "gt_abstractsettings.h"
#include "gt_settingsitem.h"
#include "gt_algorithms.h"

#include "gt_logging.h"

GtAbstractSettings::GtAbstractSettings(GtObject* parent) : GtObject(parent)
{

}

bool
GtAbstractSettings::requiresAppRestart() const
{
    return std::any_of(std::begin(m_settings), std::end(m_settings),
                       [](const GtSettingsItem* s) -> bool {
        return s? s->requiresRestart() : false;
    });
}

GtAbstractSettings::~GtAbstractSettings()
{
    qDeleteAll(m_settings);
}

void
GtAbstractSettings::restoreSettings()
{
    // own settings
    foreach (GtSettingsItem* i, m_settings)
    {
        QSettings().setValue(i->ident(), i->initValue());
    }

    // restore children
    QList<GtAbstractSettings*> list = findDirectChildren<GtAbstractSettings*>();

    foreach (GtAbstractSettings* c, list)
    {
        c->restoreSettings();
    }
}

GtSettingsItem*
GtAbstractSettings::registerSetting(const QString& ident,
                                    const QVariant& initVal)
{
    GtSettingsItem* retval = new GtSettingsItem(ident, initVal);
    m_settings.insert(ident, retval);
    return retval;
}

GtSettingsItem*
GtAbstractSettings::registerSettingRestart(const QString &ident,
                                           const QVariant &initVal)
{
    GtSettingsItem* retval = new GtSettingsItem(ident, initVal, true);
    m_settings.insert(ident, retval);
    return retval;
}

void
GtAbstractSettings::setSetting(const QString &ident, const QVariant &value)
{
    auto iter = m_settings.find(ident);
    if (iter != m_settings.end())
    {
        iter.value()->setValue(value);
    }
}

QVariant
GtAbstractSettings::getSetting(const QString &ident) const
{
    auto iter = m_settings.find(ident);
    if (iter != m_settings.end())
    {
        return iter.value()->getValue();
    }
    else
    {
        gtError().noquote()
            << tr("Cannot query setting. Setting '%1' does not exist.")
                   .arg(ident);
        return {};
    }
}

bool
GtAbstractSettings::hasSetting(const QString &ident) const
{
    auto iter = m_settings.find(ident);
    return iter != m_settings.end();
}

QStringList
GtAbstractSettings::getAllSettingIds() const
{
    QStringList l;

    gt::for_each_key(m_settings, [&l](auto&& id) {
        l.push_back(id);
    });

    return l;
}

