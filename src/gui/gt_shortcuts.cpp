/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_shortcuts.cpp
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
    auto end = tab.constKeyValueEnd();
    for (auto item = tab.constKeyValueBegin(); item != end; ++item)
    {
        // list of 2 or 3 elements
        // { shortcut, category, readonly }
        QStringList const& data = item->second;
        assert(data.size() > 1);

        // shortcut
        QKeySequence const& k = data.at(0);
        // category
        QString const& cat = data.at(1);

        // read only flag
        bool readOnly = false;
        if (data.size() > 2)
        {
            if (data.at(2) == "true")
            {
                readOnly = true;
            }
        }

        auto c = new GtShortCut(item->first, k, cat, readOnly);
        c->setParent(this);
    }
}

void
GtShortCuts::initialize(const QList<GtShortCutSettingsData>& list)
{
    for (const auto& data : list)
    {
        initialize(data);
    }
}

void
GtShortCuts::initialize(const GtShortCutSettingsData& data)
{
    if (!findShortCut(data.id, data.category))
    {
        auto c = new GtShortCut(data);
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
                             [&](const GtShortCut* c){
        return c->compare(id, category);
    });

    if (iter == std::end(list))
    {
        return nullptr;
    }

    return *iter;
}

void
GtShortCuts::emitChange()
{
    emit changed();
}

QKeySequence
GtShortCuts::getKey(const QString& id, const QString& category) const
{
    const QList<GtShortCut*>list = shortCuts();

    if (list.isEmpty())
    {
#ifndef NDEBUG
        gtWarning() << tr("No shortcuts registered!");
#endif
        return {};
    }

    using Iterator = typename QList<GtShortCut*>::const_iterator;
    Iterator iter{};

    if (category.isEmpty())
    {
        iter = std::find_if(std::begin(list), std::end(list),
                            [&](const GtShortCut* c){
            return c->compare(id);
        });
    }
    else
    {
        iter = std::find_if(std::begin(list), std::end(list),
                            [&](const GtShortCut* c){
            return c->compare(id, category);
        });
    }

    if (iter == std::end(list))
    {
#ifndef NDEBUG
        gtWarning() << tr("No shortcut registered for") << id;
#endif
        return {};
    }

    return (*iter)->key();
}
