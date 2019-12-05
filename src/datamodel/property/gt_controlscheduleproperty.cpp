/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 01.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */

#include "gt_controlscheduleproperty.h"
#include "gt_controlschedule.h"

GtControlScheduleProperty::GtControlScheduleProperty(const QString& name,
                                                     const QString& brief,
                                                     const QString& id,
                                                     GtObject* obj) :
    m_obj(obj)
{
    setObjectName(name);
    m_brief = brief;
    m_unitCategory = GtUnit::Category::None;
    m_value = id;
    m_storeMemento = false;
}

QVariant
GtControlScheduleProperty::valueToVariant(const QString& unit,
                                          bool* success) const
{
    return QVariant::fromValue(getVal(unit, success));
}

bool
GtControlScheduleProperty::setValueFromVariant(const QVariant& /*val*/,
                                             const QString& /*unit*/,
                                             bool* /*success*/)
{
    return true;
}

GtControlSchedule*
GtControlScheduleProperty::controlSchedule()
{
    if (m_obj == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

//    return m_obj->findDirectChild<GtControlSchedule*>(m_value);
    // 03.08.2016 m_obj is already the GtControlSchedule
    return qobject_cast<GtControlSchedule*>(m_obj);
}

GtObject*
GtControlScheduleProperty::object()
{
    return m_obj;
}

// 03.08.2016 test function to set GtControlSchedule* from outside new
// to m_obj
//void
//GtControlScheduleProperty::setObject(GtObject* o)
//{
////    if (m_obj != Q_NULLPTR)
////    {
////        delete m_obj;
////        m_obj = Q_NULLPTR;
////    }

//    m_obj = o;
//}
