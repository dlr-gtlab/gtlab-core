/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 05.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVariant>
#include <QFont>

#include "gt_propertycategoryitem.h"
#include "gt_propertyitem.h"
#include "gt_application.h"
#include "gt_propertymodel.h"
#include "gt_modeproperty.h"
#include "gt_propertymodeitem.h"
#include "gt_propertyobjectlinkitem.h"
#include "gt_objectlinkproperty.h"
#include "gt_propertylabelitem.h"
#include "gt_labelproperty.h"
#include "gt_filechooserproperty.h"
#include "gt_propertyfilechooseritem.h"
#include "gt_propertyitemfactory.h"

GtPropertyCategoryItem::GtPropertyCategoryItem(GtObject* scope,
                                               const QString& id,
                                               GtPropertyModel* model,
                                               GtObject* parent) :
    m_id(id),
    m_icon(gtApp->icon(QStringLiteral("arrowrightIcon.png")))
{
    setModel(model);
    setScope(scope);
    setParent(parent);
}

void
GtPropertyCategoryItem::addPropertyItem(GtAbstractProperty* property)
{
    if (property)
    {
        gtPropertyItemFactory->newItem(property, m_scope, m_model, this);
    }
}

const QString&
GtPropertyCategoryItem::categoryId() const
{
    return m_id;
}

QVariant
GtPropertyCategoryItem::data(int column, int role) const
{
    switch (role)
    {
        case Qt::DisplayRole:
        {
            if (column == 0)
            {
                return m_id;
            }

            break;
        }

        case Qt::DecorationRole:
        {
            if (column == 0)
            {
                return m_icon;
            }

            break;
        }

        case GtPropertyModel::CategoryRole:
        {
            return true;
        }

        case GtPropertyModel::MonitoringRole:
        {
            if (m_id == QStringLiteral("Monitoring"))
            {
                return true;
            }
        }
    }

    return QVariant();
}

bool
GtPropertyCategoryItem::setData(int column, const QVariant& value,
                                GtObject* /*obj*/, int role)
{
    if (column != 0)
    {
        return false;
    }

    if (role != Qt::DecorationRole)
    {
        return false;
    }

    m_icon = value.value<QIcon>();

    return true;
}
