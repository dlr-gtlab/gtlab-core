/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 05.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_modetypeproperty.h"

GtModeTypeProperty::GtModeTypeProperty(const QString& name,
                                       const QString& brief) :
    GtStringProperty(QString(), name, brief)
{
    m_storeMemento = false;
}

