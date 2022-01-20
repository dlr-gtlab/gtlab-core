/* GTlab - Gas Turbine laboratory
 * Source File: gt_shortcut.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 13.08.2021
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_shortcut.h"

GtShortCut::GtShortCut(const QString &id,
                       const QKeySequence &key,
                       const QString &cat) :
    m_id(id),
    m_key(key),
    m_cat(cat)
{

}

QString
GtShortCut::id() const
{
    return m_id;
}

void
GtShortCut::setId(const QString &id)
{
    m_id = id;
}

QKeySequence
GtShortCut::key() const
{
    return m_key;
}

void
GtShortCut::setKey(const QKeySequence &key)
{
    m_key = key;
}


QString
GtShortCut::category() const
{
    return m_cat;
}
