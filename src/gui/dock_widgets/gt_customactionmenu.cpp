/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 24.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QMenu>

#include "gt_customactionmenu.h"
#include "gt_object.h"

/// helper method to get parent QWidget (skips QMenus)
inline QWidget*
findParentWidget(QMenu*  menu)
{
    auto* parent = qobject_cast<QWidget*>(menu->parent());

    // skip if its a menu
    if (auto* m = qobject_cast<QMenu*>(parent))
    {
        return findParentWidget(m);
    }

    return parent;
}

GtCustomActionMenu::GtCustomActionMenu(const QList<GtObjectUIAction>& list,
                                       GtObject* targetObject,
                                       QObject* parentObject,
                                       QMenu* menu) :
    QObject(menu),
    m_targetObj(targetObject),
    m_parentObj(parentObject)
{
    for (const GtObjectUIAction& a : list)
    {
        assert(menu);

        // separator
        if (a.isEmpty())
        {
            menu->addSeparator();
            continue;
        }

        // visibility
        if (!a.method() ||
            (a.visibilityMethod() &&
            !a.visibilityMethod()(m_parentObj, m_targetObj)))
        {
            continue;
        }

        // text
        QAction* act = menu->addAction(a.text());

        // icon
        if (!a.icon().isNull())
        {
            act->setIcon(a.icon());
        }

        // verification
        if (a.verificationMethod())
        {
            act->setEnabled(a.verificationMethod()(m_parentObj, m_targetObj));
        }

        // shortcut
        if (!a.shortCut().isEmpty())
        {
            act->setShortcut(a.shortCut());
            act->setShortcutContext(Qt::ApplicationShortcut);

            if (QWidget* parent = findParentWidget(menu))
            {
                parent->addAction(act);
            }
        }

        connect(act, &QAction::triggered, this, [this, act](){
            onActionTrigger(act);
        });

        m_actions.insert(act, a);
    }
}

void
GtCustomActionMenu::onActionTrigger(QObject* obj)
{
    if (QAction* act = qobject_cast<QAction*>(obj))
    {
        const GtObjectUIAction& val = m_actions.value(act);
        val.method()(m_parentObj, m_targetObj);
    }
}
