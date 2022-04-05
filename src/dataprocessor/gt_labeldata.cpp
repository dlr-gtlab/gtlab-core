/* GTlab - Gas Turbine laboratory
 * Source File: gt_labeldata.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 20.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_labeldata.h"

#include "gt_label.h"

#include <QStringList>

#include <algorithm>

GtLabelData::GtLabelData()
{
    setObjectName(QStringLiteral("Label Data"));

    setUserHidden(true);
}

QStringList
GtLabelData::labelIds() const
{
    QList<GtLabel*> list = findDirectChildren<GtLabel*>();

    QStringList retval;

    foreach (GtLabel* label, list)
    {
        retval << label->objectName();
    }

    return retval;
}

bool
GtLabelData::isEmpty() const
{
    QList<GtLabel*> list = findDirectChildren<GtLabel*>();

    return list.isEmpty();
}

bool
GtLabelData::addLabel(const QString& id)
{
    if (id.isEmpty())
    {
        return false;
    }

    if (labelExists(id))
    {
        return false;
    }

    appendChild(new GtLabel(id));

    return true;
}

bool
GtLabelData::addLabel(GtLabel* label)
{
    const QString id = label->objectName();

    if (id.isEmpty())
    {
        return false;
    }

    if (labelExists(id))
    {
        return false;
    }

    appendChild(label);

    return true;
}

QList<GtLabel*>
GtLabelData::labels()
{
    return findDirectChildren<GtLabel*>();
}

bool
GtLabelData::addDefaultLabel()
{
    if (!isEmpty())
    {
        return false;
    }

    return addLabel(tr("Default"));
}

bool
GtLabelData::labelExists(const QString& id) const
{
    auto list = findDirectChildren<GtLabel*>();

    return std::any_of(std::begin(list), std::end(list), [&id](const GtLabel* label) {
        return label->objectName() == id;
    });
}

bool
GtLabelData::renameLabel(const QString& oldId, const QString& newId)
{
    if (newId.isEmpty())
    {
        return false;
    }

    if (labelExists(newId))
    {
        return false;
    }

    QList<GtLabel*> list = findDirectChildren<GtLabel*>();

    auto iter = std::find_if(std::begin(list), std::end(list),
                             [&oldId](const GtLabel * label) {
        return label->objectName() == oldId;
    });

    if (iter != std::end(list))
    {
        (*iter)->setObjectName(newId);
        return true;
    }
    else
    {
        return false;
    }
}