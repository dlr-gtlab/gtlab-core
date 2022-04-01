/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertydoublelistitem.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 24.01.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_doublelistproperty.h"

#include "gt_propertydoublelistitem.h"

GtPropertyDoubleListItem::GtPropertyDoubleListItem()
{

}

GtDoubleListProperty*
GtPropertyDoubleListItem::doubleListProperty() const
{
    return qobject_cast<GtDoubleListProperty*>(m_property);
}

QVariant
GtPropertyDoubleListItem::data(int column, int role) const
{
    GtDoubleListProperty* listProp = doubleListProperty();

    if (!listProp)
    {
        return QVariant();
    }

    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
        {
            if (column == 2)
            {
                return listProp->valuesToString();
            }
        }
    }

    return GtPropertyItem::data(column, role);
}

bool
GtPropertyDoubleListItem::setData(int column, const QVariant& value,
                                  GtObject* obj, int role)
{
    GtDoubleListProperty* listProp = doubleListProperty();

    if (!listProp)
    {
        return false;
    }

    switch (role)
    {
        case Qt::EditRole:
        {
            if (column == 2)
            {
                return listProp->setValueFromString(value.toString());
            }
        }
    }

    return GtPropertyItem::setData(column, value, obj, role);
}
