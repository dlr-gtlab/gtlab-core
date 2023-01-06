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
#include <QPainter>
#include <QStyleOptionButton>
#include <QApplication>

#include "gt_propertycategoryitem.h"
#include "gt_propertymodel.h"
#include "gt_propertyitemfactory.h"
#include "gt_icons.h"

GtPropertyCategoryItem::GtPropertyCategoryItem(GtObject* scope,
                                               const QString& id,
                                               GtPropertyModel* model,
                                               GtObject* parent) :
    m_id(id),
    m_icon(gt::gui::icon::arrowRight())
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

void
GtPropertyCategoryItem::setIsContainer(bool isContainer)
{
    m_isContainer = isContainer;
}

bool
GtPropertyCategoryItem::isContainer() const
{
    return m_isContainer;
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

        case GtPropertyModel::ContainerRole:
        {
            return m_isContainer;
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

void
GtPropertyCategoryItem::paint(QPainter* painter,
                              const QStyleOptionViewItem& option) const
{
    painter->save();
    QStyleOptionComboBox box;
    box.rect = option.rect;

    box.state = QStyle::State_Active | QStyle::State_Enabled;

    QRect r = QApplication::style()->subControlRect(QStyle::CC_ComboBox,
              &box,
              QStyle::SC_ComboBoxArrow);

    QStyleOptionViewItem opt = option;

    double dx = (r.width() - 10) / 2;
    double dy = (r.height() - 10) / 2;

    opt.rect = QRect(r.x() + dx, r.y() + dy, 10, 10);

    QApplication::style()->drawPrimitive(QStyle::PE_IndicatorArrowDown,
                                         &opt, painter);

    painter->restore();
}
