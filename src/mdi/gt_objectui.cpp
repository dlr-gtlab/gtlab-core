/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QVariant>

#include "gt_objectui.h"

#include "gt_application.h"
#include "gt_mdilauncher.h"
#include "gt_object.h"

GtObjectUI::GtObjectUI()
{

}

QVariant
GtObjectUI::specificData(GtObject* /*obj*/,
                         int /*role*/,
                         int /*column*/) const
{
    return QVariant();
}

void
GtObjectUI::addSingleAction(const QString& actionText,
                            const QString& actionIcon,
                            const QString& actionMethod,
                            const QString& actionVerification,
                            const QString& actionVisibility)
{
    m_singleActions << GtObjectUIAction(actionText, actionMethod, actionIcon,
                                        actionVerification, actionVisibility);
}

void
GtObjectUI::addSingleAction(const QString& actionText,
                            const QString& actionIcon,
                            const QString& actionMethod,
                            const QString& actionVerification)
{
    m_singleActions << GtObjectUIAction(actionText, actionMethod, actionIcon,
                                        actionVerification);
}

void
GtObjectUI::addSingleAction(const QString& actionText,
                            const QString& actionIcon,
                            const QString& actionMethod)
{
    m_singleActions << GtObjectUIAction(actionText, actionMethod, actionIcon);
}

void
GtObjectUI::addSingleAction(const QString& actionText,
                            const QString& actionMethod)
{
    m_singleActions << GtObjectUIAction(actionText, actionMethod);
}

void
GtObjectUI::addActionGroup(GtObjectUIActionGroup actionGroup)
{
    m_actionGroups.append(actionGroup);
}

void
GtObjectUI::addActionGroup(const QString& groupName,
                           const QList<GtObjectUIAction> actions)
{
    m_actionGroups.append(GtObjectUIActionGroup(groupName, actions));
}

void
GtObjectUI::addActionGroup(const QString& groupName,
                           const QString& groupIcon,
                           const QList<GtObjectUIAction> actions)
{
    m_actionGroups.append(GtObjectUIActionGroup(groupName, actions, groupIcon));
}

void
GtObjectUI::addSeparator()
{
    m_singleActions << GtObjectUIAction();
}

bool
GtObjectUI::hasValidationRegExp()
{
    return false;
}

QRegExp
GtObjectUI::validatorRegExp()
{
    return QRegExp("");
}

QIcon
GtObjectUI::icon(GtObject* /*obj*/) const
{
    return gtApp->icon(QStringLiteral("emptyIcon_16.png"));;
}

QVariant
GtObjectUI::data(GtObject* obj, int role, int column) const
{
    if (obj == Q_NULLPTR)
    {
        return QVariant();
    }

    QVariant retval = specificData(obj, role, column);

    if (retval.isValid())
    {
        return retval;
    }

    if (column == 0)
    {
        switch (role)
        {
            case Qt::DecorationRole:
            {
                return icon(obj);
            }

            case Qt::ForegroundRole:
            {
                if (obj->newlyCreated())
                {
                    return QColor(Qt::darkGreen);
                }
                else if (obj->hasChanges() || obj->hasChildChanged())
                {
                    return QColor(Qt::blue);
                }

                break;
            }
        }
    }
    else if (column == 1)
    {
        switch (role)
        {
            case Qt::DecorationRole:
            {
                if (obj->newlyCreated())
                {
                    return gtApp->icon(QStringLiteral("addIcon_16.png"));
                }
                else if (obj->hasChanges())
                {
                    return gtApp->icon(
                                QStringLiteral("exclamationIcon_16.png"));
                }
            }

            case Qt::ToolTipRole:
            {
                if (obj->newlyCreated())
                {
                    return tr("Newly created");
                }
                else if (obj->hasChanges())
                {
                    return tr("Contains changes");
                }
            }
        }
    }

    return QVariant();
}

bool
GtObjectUI::setData(GtObject* /*obj*/, int /*role*/, const QVariant& /*value*/)
{
    return false;
}

QStringList
GtObjectUI::openWith(GtObject* /*obj*/)
{
    return QStringList();
}

const QList<GtObjectUIAction>&
GtObjectUI::actions()
{
    return m_singleActions;
}

bool
GtObjectUI::hasActions()
{
    return !m_singleActions.isEmpty();
}

const QList<GtObjectUIActionGroup>
GtObjectUI::actionGroups()
{
    return m_actionGroups;
}

bool
GtObjectUI::hasActionGroups()
{
    return !m_actionGroups.isEmpty();
}

void
GtObjectUI::doubleClicked(GtObject* obj)
{
    if (obj == Q_NULLPTR)
    {
        return;
    }

    QStringList openList = openWith(obj);

    if (openList.isEmpty())
    {
        return;
    }

    gtMdiLauncher->open(openList.first(), obj);
}