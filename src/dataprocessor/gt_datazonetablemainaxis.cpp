#include "gt_datazonetablemainaxis.h"
#include <QStringList>
#include "gt_logging.h"

//GtDataZoneTableMainAxis::GtDataZoneTableMainAxis(const QString name,
//                                                 QStringList axisList):
//    m_axisTicks(axisList)
//{
//    m_name = name;
//}


GtDataZoneTableMainAxis::GtDataZoneTableMainAxis(const QString& name,
                                                 AxisType axType,
                                                 QString title)
{
    m_name = name;
    m_axType = axType;
    m_title = title;
    m_active = true;

    setObjectName(name);
}

GtDataZoneTableMainAxis::GtDataZoneTableMainAxis()
{

}

int
GtDataZoneTableMainAxis::size() const
{
    return m_axisTicks.size();
}

void
GtDataZoneTableMainAxis::setTicks(QStringList list)
{
    m_axisTicks = list;
}

QStringList
GtDataZoneTableMainAxis::ticks() const
{
    return m_axisTicks;
}

int
GtDataZoneTableMainAxis::indexOf(const QString& str, bool* ok) const
{
    // if axis does not contain this tick return 0
    if (!m_axisTicks.contains(str))
    {
        gtWarning() << "Axis " << m_name << ", does not contain tick " << str;

        if (ok)
        {
            *ok = false;
        }

        return 0;
    }

    if (ok)
    {
        *ok = true;
    }

    return m_axisTicks.indexOf(str);
}

QString
GtDataZoneTableMainAxis::name() const
{
    return m_name;
}

void GtDataZoneTableMainAxis::setName(const QString& name)
{
    m_name = name;
}

QString
GtDataZoneTableMainAxis::title() const
{
    return m_title;
}

void
GtDataZoneTableMainAxis::setTitle(const QString &title)
{
    m_title = title;
}

bool
GtDataZoneTableMainAxis::active() const
{
    return m_active;
}

void
GtDataZoneTableMainAxis::setActive(bool active)
{
    m_active = active;
}

GtDataZoneTableMainAxis::AxisType
GtDataZoneTableMainAxis::axisType() const
{
    return m_axType;
}
