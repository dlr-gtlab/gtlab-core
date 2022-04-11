/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>
#include <QFont>
#include <QVariant>

#include "gt_objectui.h"

#include "gt_icons.h"
#include "gt_mdilauncher.h"
#include "gt_object.h"
#include "gt_regexp.h"
#include "gt_colors.h"

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
                            const QString& actionVerification,
                            const QString& actionVisibility,
                            const QKeySequence& shortcut)
{
    m_singleActions << GtObjectUIAction(actionText, actionMethod, actionIcon,
                                        actionVerification, actionVisibility,
                                        shortcut);
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
GtObjectUI::addSingleAction(
        const QString& actionText,  const QString& actionIcon,
        const QString& actionMethod, const QString& actionVerification,
        const QKeySequence& shortcut)
{
    m_singleActions << GtObjectUIAction(actionText, actionMethod, actionIcon,
                                        actionVerification, QString(),
                                        shortcut);
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
                            const QString& actionIcon,
                            const QString& actionMethod,
                            const QKeySequence& shortcut)
{
    m_singleActions << GtObjectUIAction(actionText, actionMethod, actionIcon,
                                        QString(), QString(), shortcut);
}


void
GtObjectUI::addSingleAction(const QString& actionText,
                            const QString& actionMethod)
{
    m_singleActions << GtObjectUIAction(actionText, actionMethod);
}

void
GtObjectUI::addSingleAction(const QString& actionText,
                            const QString& actionMethod,
                            const QKeySequence& shortcut)
{
    m_singleActions << GtObjectUIAction(actionText, actionMethod,
                                        QString(), QString(), QString(),
                                        shortcut);
}

void
GtObjectUI::addActionGroup(const GtObjectUIActionGroup& actionGroup)
{
    m_actionGroups.append(actionGroup);
}

void
GtObjectUI::addActionGroup(const QString& groupName,
                           const QList<GtObjectUIAction>& actions)
{
    m_actionGroups.append(GtObjectUIActionGroup(groupName, actions));
}

void
GtObjectUI::addActionGroup(const QString& groupName,
                           const QString& groupIcon,
                           const QList<GtObjectUIAction>& actions)
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
    return true;
}

QRegExp
GtObjectUI::validatorRegExp()
{
    return GtRegExp::onlyLettersAndNumbersAndSpace();
}

QIcon
GtObjectUI::icon(GtObject* /*obj*/) const
{
    return GtGUI::Icon::empty16();
}

QVariant
GtObjectUI::data(GtObject* obj, int role, int column) const
{
    if (!obj)
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
            case Qt::ToolTipRole:
            {
                if (obj->isDummy())
                {
                    return "Object of unknown class!";
                }

                return QVariant();
            }

            case Qt::DecorationRole:
            {
                if (obj->isDummy())
                {
                    return GtGUI::Icon::unknown();
                }

                return icon(obj);
            }

            case Qt::ForegroundRole:
            {
                if (obj->newlyCreated())
                {
                    return GtGUI::Color::newObjectForground();
                }
                else if (obj->hasChanges() || obj->hasChildChanged())
                {
                    return GtGUI::Color::changedObjectForground();
                }

                break;
            }

            case Qt::BackgroundColorRole:
            {
                if (obj->isDummy())
                {
                    return GtGUI::Color::dummyObjectBackground();
                }
                break;
            }
            case Qt::FontRole:
            {
                if (obj->isDummy())
                {
                    QFont font;
                    font.setBold(true);
                    font.setItalic(true);
                    return font;
                }
            }
        }
    }
    else if (column == 1)
    {
        switch (role)
        {
            case Qt::DecorationRole:
            {
                if (obj->isDummy() || obj->hasDummyChildren())
                {
                    return GtGUI::Icon::unknown();
                }

                if (obj->newlyCreated())
                {
                    return GtGUI::Icon::add16();
                }

                if (obj->hasChanges())
                {
                    return GtGUI::Icon::exclamation16();
                }
                break;
            }

            case Qt::ToolTipRole:
            {
                if (obj->newlyCreated())
                {
                    return tr("Newly created");
                }

                if (obj->hasChanges())
                {
                    return tr("Contains changes");
                }

                return QVariant();
            }

            case Qt::BackgroundColorRole:
            {
                if (obj->isDummy())
                {
                    return GtGUI::Color::dummyObjectBackground();
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
    if (!obj)
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
