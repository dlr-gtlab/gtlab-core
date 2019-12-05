/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 09.12.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_resultsmodel.h"
#include "gt_object.h"
#include "gt_explorermodel.h"
#include "gt_abstractdatazone.h"

GtResultsModel::GtResultsModel(GtExplorerModel* parent) :
    GtTreeFilterModel(parent),
    m_explorerModel(parent)
{

}

bool
GtResultsModel::filterAcceptsRow(int source_row,
                                 const QModelIndex& source_parent) const
{
    if (!source_parent.isValid())
    {
        return true;
    }

    bool retval = GtTreeFilterModel::filterAcceptsRow(source_row,
                                                      source_parent);

    if (retval == false)
    {
        return false;
    }

    if (m_explorerModel != Q_NULLPTR)
    {
        GtObject* obj = m_explorerModel->objectFromIndex(source_parent);

        if (obj != Q_NULLPTR)
        {
            GtObjectList childs = obj->findDirectChildren<GtObject*>();

            if (childs.size() > source_row)
            {
                GtObject* c = childs.at(source_row);

                if (c == Q_NULLPTR)
                {
                    return false;
                }

                if (qobject_cast<GtAbstractDataZone*>(c))
                {
                    return true;
                }

                GtAbstractDataZone* zone = c->findChild<GtAbstractDataZone*>();
                if (zone != Q_NULLPTR)
                {
                    return true;
                }
            }

            return false;
        }
    }

    return false;
}

QModelIndexList
GtResultsModel::getPersistentIndexList()
{
    return persistentIndexList();
}

