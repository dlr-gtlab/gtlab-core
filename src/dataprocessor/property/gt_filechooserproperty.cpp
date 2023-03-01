/* GTlab - Gas Turbine laboratory
 * Source File: gt_filechooserproperty.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 19.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_filechooserproperty.h"

#include "gt_regexp.h"

GtFileChooserProperty::GtFileChooserProperty(const QString& id,
                                             const QString& name,
                                             const QString& brief,
                                             const QStringList& fileFilter) :
    GtStringProperty(id, name, brief, QString()),
    m_filter(fileFilter)
{

}

const QStringList&
GtFileChooserProperty::filter()
{
    return m_filter;
}

QString buildDialogFileFilter(const QStringList &filterList)
{

    if (filterList.empty())
    {
        return "";
    }

    auto re = gt::re::forDialogFileFilters();
    auto isQtFilter = [&re](const QString& filter)
    {
        return re.indexIn(filter) >= 0;
    };

    QString result;

    for (int i = 0; i < filterList.size()-1; ++i)
    {
        auto filter = filterList[i];
        auto&& nextFilter = filterList[i+1];

        QString delimiter = ";;";
        if (!isQtFilter(filter))
        {
            if (!isQtFilter(nextFilter)) delimiter = "; ";
            filter = "*" + filter;
        }

        result += filter + delimiter;
    }

    QString lastFilter = filterList.last();
    if (!isQtFilter(lastFilter)) lastFilter = "*" + lastFilter;

    result += lastFilter;

    return result;
}
