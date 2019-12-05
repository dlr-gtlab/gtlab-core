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

GtAbstractSettings::GtAbstractSettings(GtObject* parent) : GtObject(parent)
{

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
    m_settings.append(retval);
    return retval;
}

