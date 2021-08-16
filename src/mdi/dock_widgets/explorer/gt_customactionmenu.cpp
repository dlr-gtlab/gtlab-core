/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 24.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QMenu>
#include <QSignalMapper>

#include "gt_customactionmenu.h"
#include "gt_object.h"
#include "gt_objectui.h"
#include "gt_application.h"
#include "gt_logging.h"

GtCustomActionMenu::GtCustomActionMenu(const QList<GtObjectUIAction>& list,
                                       GtObject* targetObj,
                                       QObject* parentObject,
                                       QMenu* menu) :
    QObject(menu),
    m_signalMapper(new QSignalMapper(this)),
    m_targetObj(targetObj),
    m_parentObj(parentObject)
{
    connect(m_signalMapper, SIGNAL(mapped(QObject*)),
            SLOT(onActionTrigger(QObject*)));

    foreach (const GtObjectUIAction& a, list)
    {
        if (a.text().isEmpty())
        {
            menu->addSeparator();
            continue;
        }

        if (!a.visibilityMethod().isEmpty())
        {
            bool visible = false;
            QMetaObject::invokeMethod(
                        m_parentObj, a.visibilityMethod().toLatin1(),
                        Q_RETURN_ARG(bool, visible),
                        Q_ARG(GtObject*, m_targetObj));

            if (!visible)
            {
                continue;
            }
        }

        QAction* act = Q_NULLPTR;

        if (a.icon().isEmpty())
        {
            act = menu->addAction(a.text());
        }
        else
        {
            act = menu->addAction(gtApp->icon(a.icon()), a.text());
        }

        // verification
        if (!a.verificationMethod().isEmpty())
        {
            bool verified = false;
            QMetaObject::invokeMethod(m_parentObj,
                                      a.verificationMethod().toLatin1(),
                                      Q_RETURN_ARG(bool, verified),
                                      Q_ARG(GtObject*, m_targetObj));
            act->setEnabled(verified);
        }

        if (!a.shortCut().isEmpty())
        {
            act->setShortcut(a.shortCut());
            act->setShortcutContext(Qt::ApplicationShortcut);

            QWidget* parent = qobject_cast<QWidget*>(menu->parent());
            if (parent != nullptr)
            {
                parent->addAction(act);
            }
        }

        connect(act, SIGNAL(triggered(bool)), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(act, act);

        m_actions.insert(act, a);
    }
}


void
GtCustomActionMenu::onActionTrigger(QObject* obj)
{
    QAction* act = qobject_cast<QAction*>(obj);

    if (act == Q_NULLPTR)
    {
        return;
    }

    const GtObjectUIAction& val = m_actions.value(act);

    if (!QMetaObject::invokeMethod(m_parentObj, val.method().toLatin1(),
                                   Q_ARG(GtObject*, m_targetObj)))
    {
        gtWarning() << tr("Could not invoke method!") << " ("
                    << val.method() << ")";
    }
}
