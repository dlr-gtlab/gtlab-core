// SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
//
// SPDX-License-Identifier: MPL-2.0+

/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 10.11.2017
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_examplesentry.h"

GtExamplesEntry::GtExamplesEntry() :
    m_description(QString(""))
{

}

GtExamplesEntry::~GtExamplesEntry()
{

}

QString
GtExamplesEntry::dirPath() const
{
    return m_dirPath;
}

void
GtExamplesEntry::setDirPath(const QString &dirPath)
{
    m_dirPath = dirPath;
}

QString
GtExamplesEntry::pixmapPath() const
{
    return m_pixmapPath;
}

void
GtExamplesEntry::setPixmapPath(const QString& pixmapPath)
{
    m_pixmapPath = pixmapPath;
}

QString
GtExamplesEntry::category() const
{
    return m_category;
}

void
GtExamplesEntry::setCategory(const QString &category)
{
    m_category = category;
}

QString
GtExamplesEntry::name() const
{
    return m_name;
}

void
GtExamplesEntry::setName(const QString &name)
{
    m_name = name;
}

QString
GtExamplesEntry::description() const
{
    return m_description;
}

void
GtExamplesEntry::setDescription(const QString &description)
{
    m_description = description;
}
