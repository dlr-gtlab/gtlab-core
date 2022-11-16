/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 30.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_settingsitem.h"
#include <QSettings>

GtSettingsItem::GtSettingsItem(const QString& ident,
                               const QVariant& initVal,
                               bool needsRestart) :
    m_ident(ident),
    m_initValue(initVal),
    changesRequiresRestart(needsRestart)
{

}

const QString&
GtSettingsItem::ident() const
{
    return m_ident;
}

const QVariant&
GtSettingsItem::initValue() const
{
    return m_initValue;
}

void
GtSettingsItem::setValue(const QVariant &value)
{
    if (value != getValue())
    {
        QSettings().setValue(ident(), value);
        hasChanged = true;
    }
    else
    {
        hasChanged = false;
    }
}

QVariant GtSettingsItem::getValue() const
{
    return QSettings().value(ident(), initValue());
}

bool GtSettingsItem::requiresRestart() const
{
    return changesRequiresRestart && hasChanged;
}

