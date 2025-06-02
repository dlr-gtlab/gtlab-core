/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 17.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_openwithmenu.h"
#include "gt_mdilauncher.h"

GtOpenWithMenu::GtOpenWithMenu(const QStringList& list,
                               GtObject* obj,
                               QWidget* parent) :
    QMenu(parent),
    m_obj(obj)
{
    setTitle(QStringLiteral("Open With"));

    for (const QString& str : list)
    {
        QString name = cleanMenuName(str);

        QAction* act = addAction(name);

        connect(act, &QAction::triggered, this, [this, act](){
            onActionTrigger(act);
        });

        m_actions.insert(act, str);
    }
}

void
GtOpenWithMenu::onActionTrigger(QObject* obj)
{
    QAction* act = qobject_cast<QAction*>(obj);

    if (!act)
    {
        return;
    }

    const QString str = m_actions.value(act);

    if (str.isEmpty())
    {
        return;
    }

    gtMdiLauncher->open(str, m_obj);
}

QString
GtOpenWithMenu::cleanMenuName(const QString& className)
{
    if (className == "GtdPreDesignPlot")
    {
        return "Pre Design Plot";
    }

    if (className == "GtdPreDesignPlot3D")
    {
        return "3D Pre Design Plot";
    }

    if (className == "GtdProfilePlot")
    {
        return "Profile Plot";
    }

    if (className == "GtdComponentEditor")
    {
        return "Component Editor";
    }

    return className;
}
