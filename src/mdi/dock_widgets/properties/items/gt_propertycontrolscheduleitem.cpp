/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 02.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */

#include "gt_propertycontrolscheduleitem.h"
#include "gt_controlscheduleproperty.h"
#include "gt_propertycontrolscheduleeditor.h"
#include "gt_controlschedule.h"

GtPropertyControlScheduleItem::GtPropertyControlScheduleItem()
{

}

QVariant
GtPropertyControlScheduleItem::data(int column, int role) const
{
    if (column < 0 || column >= 3)
    {
        return QVariant();
    }

    if (m_property == Q_NULLPTR)
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
                GtControlSchedule* cs = csProperty()->controlSchedule();

                if (cs == Q_NULLPTR)
                {
                    return QStringLiteral("-");
                }

                return cs->getDescription();
            }
        }
    }

    return GtPropertyItem::data(column, role);
}

bool
GtPropertyControlScheduleItem::setData(int column,
                                       const QVariant& value,
                                       GtObject* obj,
                                       int role)
{
    if (column == 0)
    {
        GtPropertyItem::setData(column, value, obj, role);
        return true;
    }

    if (column != 2)
    {
        return false;
    }

    if (role != Qt::EditRole)
    {
        return false;
    }

    return GtPropertyItem::setData(column, value, obj, role);
}

GtControlScheduleProperty*
GtPropertyControlScheduleItem::csProperty() const
{
    return qobject_cast<GtControlScheduleProperty*>(m_property);
}

QWidget*
GtPropertyControlScheduleItem::editorWidget(
    QWidget* parent,
    const GtPropertyValueDelegate* /*delegate*/) const
{
    GtPropertyControlScheduleEditor* e =
        new GtPropertyControlScheduleEditor(parent);

    return e;
}

void GtPropertyControlScheduleItem::setEditorData(QWidget* editor,
                                                  QVariant& /*var*/) const
{
    GtPropertyControlScheduleEditor* e =
        static_cast<GtPropertyControlScheduleEditor*>(editor);

    e->setControlScheduleProperty(csProperty());
}

void
GtPropertyControlScheduleItem::setModelData(QWidget* /*editor*/,
                                            QAbstractItemModel* /*model*/,
                                            const QModelIndex& /*index*/) const
{

}
