/* GTlab - Gas Turbine laboratory
 * Source File: gt_externalizationsettings.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 04.08.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_externalizationsettings.h"

GtExternalizationSettings::GtExternalizationSettings() :
    m_autoExternalize(true),
    m_enableExternalization(true)
{

}

GtExternalizationSettings*
GtExternalizationSettings::instance()
{
    static GtExternalizationSettings* self = new GtExternalizationSettings();

    return self;
}

bool
GtExternalizationSettings::isExternalizationEnabled() const
{
#if GT_H5
    return m_enableExternalization;
#else
    return false;
#endif
}

bool
GtExternalizationSettings::autoExternalizeOnSave() const
{
#if GT_H5
    return m_autoExternalize;
#else
    return false;
#endif
}

void
GtExternalizationSettings::onEnbaleExternalizationChanged(const QVariant& var)
{
    m_enableExternalization = var.toBool();
}

void
GtExternalizationSettings::onAutoExternalizationChanged(const QVariant& var)
{
    m_autoExternalize = var.toBool();
}
