/* GTlab - Gas Turbine laboratory
 * Source File: gt_savefilenameproperty.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 12.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_savefilenameproperty.h"

GtSaveFileNameProperty::GtSaveFileNameProperty(const QString& id,
                                               const QString& name,
                                               const QString& brief,
                                               const QStringList& fileFilter,
                                               const QString& initFileName) :
    GtFileChooserProperty(id, name, brief, fileFilter),
    m_initFileName(initFileName)
{

}

const QString&
GtSaveFileNameProperty::initFileName()
{
    return m_initFileName;
}
