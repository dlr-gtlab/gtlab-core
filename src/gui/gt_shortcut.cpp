/* GTlab - Gas Turbine laboratory
 * Source File: gt_shortcut.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 13.08.2021
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_shortcut.h"

GtShortCut::GtShortCut(const QString& id,
                       const QKeySequence& key,
                       const QString& cat,
                       const bool isReadOnly)
{  
    m_data.id = id;
    m_data.category = cat;
    m_data.shortCut = key;
    m_data.isReadOnly = isReadOnly;
}

GtShortCut::GtShortCut(GtShortCutSettingsData data) :
    m_data(std::move(data))
{ }

QString
GtShortCut::id() const
{
    return m_data.id;
}

void
GtShortCut::setId(const QString &id)
{
    m_data.id = id;
}

QKeySequence
GtShortCut::key() const
{
    return m_data.shortCut;
}

void
GtShortCut::setKey(const QKeySequence &key)
{
    m_data.shortCut = key;
}

QString
GtShortCut::category() const
{
    return m_data.category;
}

bool
GtShortCut::isReadOnly() const
{
    return m_data.isReadOnly;
}

bool
GtShortCut::compare(const QString& id) const
{
    return m_data.id.compare(id, Qt::CaseInsensitive) == 0;
}

bool
GtShortCut::compare(const QString& id, const QString& cat) const
{
    return m_data.id.compare(id, Qt::CaseInsensitive) == 0 &&
           m_data.category.compare(cat, Qt::CaseInsensitive) == 0;
}
