/* GTlab - Gas Turbine laboratory
 * Source File: gt_accessselectionproperty.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 01.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_accessmanager.h"
#include "gt_accessgroup.h"

#include "gt_accessselectionproperty.h"

GtAccessSelectionProperty::GtAccessSelectionProperty(const QString& ident,
                                                     const QString& name,
                                                     const QString& accessId,
                                                     const QString& brief) :
    GtStringProperty(ident, name, brief),
    m_accessId(accessId)
{

}

const QString&
GtAccessSelectionProperty::accessId() const
{
    return m_accessId;
}

GtAccessData
GtAccessSelectionProperty::accessData() const
{
    GtAccessGroup* group = gtAccessManager->accessGroup(m_accessId);

    if (!group)
    {
        return GtAccessData(QString());
    }

    const auto& data = group->accessData();
    auto iter = std::find_if(std::begin(data), std::end(data),
                             [this](const GtAccessData& data) {
        return data.host() == getVal();
    });

    return iter != std::end(data) ? *iter : GtAccessData(QString());
}
