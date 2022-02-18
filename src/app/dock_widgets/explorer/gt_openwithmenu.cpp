/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 17.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QSignalMapper>

#include "gt_openwithmenu.h"
#include "gt_mdilauncher.h"

GtOpenWithMenu::GtOpenWithMenu(const QStringList& list, GtObject* obj,
                               QWidget* parent) :
    QMenu(parent),
    m_signalMapper(new QSignalMapper(this)),
    m_obj(obj)
{
    setTitle(QStringLiteral("Open With"));

    connect(m_signalMapper, SIGNAL(mapped(QObject*)),
            SLOT(onActionTrigger(QObject*)));

    foreach (const QString& str, list)
    {
        QString name = cleanMenuName(str);

        QAction* act = addAction(name);

        connect(act, SIGNAL(triggered(bool)), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(act, act);

        m_actions.insert(act, str);
    }
}

void
GtOpenWithMenu::onActionTrigger(QObject* obj)
{
    QAction* act = qobject_cast<QAction*>(obj);

    if (act == Q_NULLPTR)
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
