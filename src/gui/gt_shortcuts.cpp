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

#include <algorithm>

GtShortCuts::GtShortCuts(QObject* parent)
{
    setParent(parent);
}

void
GtShortCuts::initialize(const QMap<QString, QStringList>& tab)
{
    foreach (QString const& key, tab.keys())
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
    const QList<GtShortCut*> list = shortCuts();

    auto iter = std::find_if(std::begin(list), std::end(list),
        [&id, &category](const GtShortCut* c) {
            return c->id() == id &&  c->category() == category;
        });
    if (iter != std::end(list))
    {
        return *iter;
    }
    else
    {
        return nullptr;
    }
}

QKeySequence
GtShortCuts::getKey(const QString& id) const
{
    const QList<GtShortCut*>list = shortCuts();

    if (shortCuts().isEmpty())
    {
        gtWarning() << "No shortCut registered";

        return QKeySequence{};
    }

    auto iter = std::find_if(std::begin(list), std::end(list),
                             [&id](const GtShortCut* c) {
        return c->id() == id;
    });

    return iter != std::end(list) ? (*iter)->key() : QKeySequence{};
}
