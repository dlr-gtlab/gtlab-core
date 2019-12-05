/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 30.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_settingsitem.h"

GtSettingsItem::GtSettingsItem(const QString& ident, const QVariant& initVal) :
    m_ident(ident), m_initValue(initVal)
{

}

const QString&
GtSettingsItem::ident()
{
    return m_ident;
}

const QVariant&
GtSettingsItem::initValue()
{
    return m_initValue;
}

