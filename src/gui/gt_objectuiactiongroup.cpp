/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectuiactiongroup.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 28.09.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_objectuiactiongroup.h"

GtObjectUIActionGroup::GtObjectUIActionGroup()
{

}

GtObjectUIActionGroup::GtObjectUIActionGroup(
        const QString& groupName,
        const QList<GtObjectUIAction>& actions,
        const QString& icon) :
   m_name(groupName), m_icon(icon)
{
    foreach(GtObjectUIAction action, actions)
    {
        m_actions.append(action);
    }
}

const QList<GtObjectUIAction>
GtObjectUIActionGroup::actions()
{
    return m_actions;
}

const QString
GtObjectUIActionGroup::name()
{
    return m_name;
}

const QString
GtObjectUIActionGroup::icon()
{
    return m_icon;
}
