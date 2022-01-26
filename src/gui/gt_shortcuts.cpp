/* GTlab - Gas Turbine laboratory
 * Source File: gt_shortcuts.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 13.08.2021
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_shortcuts.h"
#include "gt_shortcut.h"
#include "gt_logging.h"

GtShortCuts::GtShortCuts(QObject* parent)
{
    setParent(parent);
}

void
GtShortCuts::initialize(const QMap<QString, QStringList>& tab)
{    
    for (QString key : tab.keys())
    {
        QKeySequence k = QKeySequence(tab.value(key).first());
        GtShortCut* c = new GtShortCut(key, k, tab.value(key).at(1));
        c->setParent(this);
    }
}


bool
GtShortCuts::isEmpty() const
{
    return shortCuts().isEmpty();
}

QList<GtShortCut*>
GtShortCuts::shortCuts() const
{
    return findChildren<GtShortCut*>();
}

GtShortCut*
GtShortCuts::findShortCut(const QString& id, const QString& category) const
{
    QList<GtShortCut*> list = shortCuts();

    GtShortCut* retVal = Q_NULLPTR;
    for (GtShortCut* c : list)
    {
        if (c->id() == id)
        {
            if (c->category() == category)
            {
                retVal = c;
            }
        }
    }

    return retVal;
}

QKeySequence
GtShortCuts::getKey(const QString& id) const
{
    QList<GtShortCut*>list = shortCuts();

    if (shortCuts().isEmpty())
    {
        gtWarning() << "No shortCut registered";

        return false;
    }

    for (GtShortCut* c : list)
    {
        if (c->id() == id)
        {
            return c->key();
        }
    }

    return QKeySequence();
}
