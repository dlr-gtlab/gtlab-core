/* GTlab - Gas Turbine laboratory
 * Source File: gt_filechooserproperty.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_filechooserproperty.h"

GtFileChooserProperty::GtFileChooserProperty(const QString& id,
                                             const QString& name,
                                             const QString& brief,
                                             const QStringList& fileFilter) :
    GtStringProperty(id, name, brief, QString()),
    m_filter(fileFilter)
{

}

const QStringList&
GtFileChooserProperty::filter()
{
    return m_filter;
}
