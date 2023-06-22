/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_savefilenameproperty.cpp
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
