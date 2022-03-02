/* GTlab - Gas Turbine laboratory
 * Source File: gt_tablegroup.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 13.04.2018
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */

#include "gt_tablegroup.h"

GtTableGroup::GtTableGroup(GtObject* parent) :
    GtObjectGroup(parent)
{
    setUserHidden(true);
}

QList<GtTable*>
GtTableGroup::tables()
{
    return findDirectChildren<GtTable*>();
}

GtTable*
GtTableGroup::table(const QString& name)
{
    if (tableNames().contains(name))
    {
        return findDirectChild<GtTable*>(name);
    }

    return nullptr;
}

QStringList
GtTableGroup::tableNames()
{
    QStringList retval;

    for (GtTable* s : tables())
    {
        if (!s)
        {
            continue;
        }

        retval.append(s->objectName());
    }

    return retval;
}
