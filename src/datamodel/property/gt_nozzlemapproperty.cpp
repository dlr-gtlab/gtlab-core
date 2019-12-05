#include "gt_nozzlemapproperty.h"
#include "gt_nozzlemap.h"

GtNozzleMapProperty::GtNozzleMapProperty(const QString& name,
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

GtNozzleMap*
GtNozzleMapProperty::nozzleMap()
{
    if (m_obj == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    // maybe using QString 'm_value' as argument
    return m_obj->findDirectChild<GtNozzleMap*>();
}

GtObject*
GtNozzleMapProperty::object()
{
    return m_obj;
}

QVariant
GtNozzleMapProperty::valueToVariant(const QString &unit, bool *success) const
{
    return QVariant::fromValue(getVal(unit, success));
}

bool
GtNozzleMapProperty::setValueFromVariant(const QVariant& /*val*/,
                                         const QString& /*unit*/,
                                         bool* /*success*/)
{
    return true;
}
