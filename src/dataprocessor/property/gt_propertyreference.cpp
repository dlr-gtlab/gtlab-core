/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_propertyreference.cpp
 *
 *  Author: Martin Siggel (AT-TW)
 *  Tel.: +49 2203 601 2264
 */


#include "gt_propertyreference.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"

#include "gt_logging.h"

#include <regex>

GtPropertyReference::GtPropertyReference(QString propertyId) :
    m_propertyId(std::move(propertyId))
{
}

GtPropertyReference::GtPropertyReference(QString containerId,
                                         QString entryId,
                                         QString propertyID) :
    m_propertyId(std::move(propertyID)),
    m_containerId(std::move(containerId)),
    m_entryId(std::move(entryId))
{
}

GtPropertyReference
GtPropertyReference::fromString(QString s, bool& ok)
{
    const std::string str = std::move(s).toStdString();

    // expression to match an id string
    const auto idExpr = QString(R"([\w,\-,\s,{,}]+)");
    // expressio to match the properties including those in arrays
    const auto propExpr = QString(R"(^((%1)\[(%2)\].)?(%3)$)")
                              .arg(idExpr, idExpr, idExpr);

    const auto re = std::regex(propExpr.toStdString());

    auto results = std::match_results<std::string::const_iterator>();
    if (std::regex_search(str, results, re))
    {
        ok = true;

        const QString containerID = results[2].str().c_str();
        const QString entryID =  results[3].str().c_str();
        const QString propID =  results[4].str().c_str();

        assert(containerID.isEmpty() == entryID.isEmpty());

        if (containerID.isEmpty())
        {
            return GtPropertyReference(propID);
        }
        else {
            return GtPropertyReference(std::move(containerID),
                                       std::move(entryID),
                                       std::move(propID));
        }
    }
    else {
        ok = false;
        return GtPropertyReference("_invalid_prop_");
    }
}

QString
GtPropertyReference::toString() const
{
    assert(m_containerId.isEmpty() == m_entryId.isEmpty());

    if (!m_containerId.isEmpty())
    {
        return QString("%1[%2].%3").arg(m_containerId, m_entryId, m_propertyId);
    }
    else
    {
        return m_propertyId;
    }
}

const QString &
GtPropertyReference::propertyId() const
{
    return m_propertyId;
}

const QString &
GtPropertyReference::containerId() const
{
    return m_containerId;
}

const QString &
GtPropertyReference::entryId() const
{
    return m_entryId;
}

GtAbstractProperty *
GtPropertyReference::resolve(GtObject & obj) const
{
    auto errorMsg = [this, &obj]() {
        gtError().noquote()
            << QString("Property '%1' cannot be resolved in object '%2'.")
                         .arg(toString(), obj.uuid());
        return nullptr;
    };

    if (m_containerId.isEmpty())
    {
        auto prop = obj.findProperty(m_propertyId);
        if (!prop) return errorMsg();

        return prop;
    }
    else
    {
        auto container = obj.findPropertyContainer(m_containerId);
        if (!container) return errorMsg();

        auto entryIter = container->findEntry(m_entryId);
        if (entryIter == container->end()) return errorMsg();

        auto prop = entryIter->findProperty(m_propertyId);

        if (!prop) return errorMsg();

        return prop;
    }
}
