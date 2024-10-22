/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
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
#include "gt_application.h"

class GtObjectUI::Impl
{
public:
    QString m_regExpHint{tr("Only letters, numbers and spaces are allowed to "
                            "be used for the renaming")};

    bool m_regExpCheckWhileModification{true};
};

GtObjectUI::GtObjectUI() :
    m_pimpl{std::make_unique<Impl>()}
{

}

GtObjectUI::~GtObjectUI() = default;

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


GtObjectUIAction&
GtObjectUI::addSingleAction(const QString& actionText,
                            const QString& actionMethod)
{
    m_singleActions << makeSingleAction(actionText, actionMethod);
    return m_singleActions.last();
}


GtObjectUIAction&
GtObjectUI::addSingleAction(const QString& actionText,
                            ActionFunction actionMethod)
{
    m_singleActions << makeSingleAction(actionText, std::move(actionMethod));
    return m_singleActions.last();
}

GtObjectUIAction
GtObjectUI::makeSingleAction(const QString& actionText,
                             const QString& actionMethod)
{
    return GtObjectUIAction(actionText, actionMethod);
}

GtObjectUIAction
GtObjectUI::makeSingleAction(const QString& actionText,
                             ActionFunction actionMethod)
{
    // parent object not needed here
    return gt::gui::makeAction(actionText, std::move(actionMethod));
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


GtObjectUIActionGroup&
GtObjectUI::addActionGroup(const QString& groupName, int sizeHint)
{
    m_actionGroups << gt::gui::makeActionGroup(groupName, sizeHint);
    return m_actionGroups.last();
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

QKeySequence
GtObjectUI::registerShortCut(const QString& id,
                             const QKeySequence& k,
                             bool readOnly)
{
    const QMetaObject* m = metaObject();
    return registerShortCut({id, m->className(), k, readOnly});
}

QKeySequence
GtObjectUI::registerShortCut(const GtShortCutSettingsData &data)
{
    gtApp->extendShortCuts(data);
    return getShortCut(data.id);
}

void
GtObjectUI::registerShortCuts(const QList<GtShortCutSettingsData>& list)
{
    gtApp->extendShortCuts(list);
}

QKeySequence
GtObjectUI::getShortCut(const QString &id)
{
    const QMetaObject* m = metaObject();
    return gtApp->getShortCutSequence(id, m->className());
}

void
GtObjectUI::setRegExpHint(const QString& hint)
{
    m_pimpl->m_regExpHint = hint;
}

void
GtObjectUI::setRegExpCheckWhileModificationEnabled(bool val)
{
    m_pimpl->m_regExpCheckWhileModification = val;
}

bool
GtObjectUI::hasValidationRegExp()
{
    return true;
}

QRegExp
GtObjectUI::validatorRegExp()
{
    return gt::re::onlyLettersAndNumbersAndSpace();
}

bool
GtObjectUI::hasValidationRegExp(GtObject* /*obj*/)
{
     return hasValidationRegExp();
}

QRegExp
GtObjectUI::validatorRegExp(GtObject* /*obj*/)
{
    return validatorRegExp();
}

QString
GtObjectUI::regExpHint(GtObject* /*obj*/)
{
    return m_pimpl->m_regExpHint;
}

bool
GtObjectUI::regExpCheckWhileModificationEnabled(GtObject* /*obj*/)
{
    return m_pimpl->m_regExpCheckWhileModification;
}

QIcon
GtObjectUI::icon(GtObject* /*obj*/) const
{
    return gt::gui::icon::objectEmpty();
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
                    return gt::gui::icon::objectUnknown();
                }

                return icon(obj);
            }

            case Qt::ForegroundRole:
            {
                if (obj->newlyCreated())
                {
                    return gt::gui::color::newObjectForeground();
                }
                else if (obj->hasChanges() || obj->hasChildChanged())
                {
                    return gt::gui::color::changedObjectForeground();
                }

                break;
            }

            case Qt::BackgroundRole:
            {
                if (obj->isDummy())
                {
                    return gt::gui::color::dummyObjectBackground();
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
                    return gt::gui::icon::objectUnknown();
                }

                if (obj->newlyCreated())
                {
                    return gt::gui::colorize(gt::gui::icon::add(),
                                             gt::gui::color::newObjectForeground);
                }

                if (obj->hasChanges())
                {
                    return gt::gui::colorize(gt::gui::icon::exclamationmark(),
                                             gt::gui::color::changedObjectForeground);
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

            case Qt::BackgroundRole:
            {
                if (obj->isDummy())
                {
                    return gt::gui::color::dummyObjectBackground();
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
