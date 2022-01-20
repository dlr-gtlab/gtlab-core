/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractcollectionitem.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 02.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_collectionitem.h"

GtCollectionItem::GtCollectionItem() : m_version(0.)
{

}

QString
GtCollectionItem::description() const
{
    return m_description;
}

bool
GtCollectionItem::isValid() const
{
    if (m_ident.isEmpty())
    {
        return false;
    }

    if (m_uuid.isEmpty())
    {
        return false;
    }

    if (m_version < 0.)
    {
        return false;
    }

    if (m_files.isEmpty())
    {
        return false;
    }

    return true;
}

QStringList
GtCollectionItem::propertyIds() const
{
    return m_properties.keys();
}

QString
GtCollectionItem::localPath() const
{
    return m_localPath;
}

QString
GtCollectionItem::ident() const
{
    return m_ident;
}

QString
GtCollectionItem::uuid() const
{
    return m_uuid;
}

double
GtCollectionItem::version() const
{
    return m_version;
}

QStringList
GtCollectionItem::files() const
{
    return m_files;
}

QVariant
GtCollectionItem::property(const QString& id) const
{
    if (!m_properties.contains(id))
    {
        return QVariant();
    }

    return m_properties.value(id);
}
