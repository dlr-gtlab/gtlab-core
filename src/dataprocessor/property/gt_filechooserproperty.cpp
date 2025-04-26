/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_filechooserproperty.cpp
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

QString
gt::detail::buildFileDialogFileFilter(const QStringList &filterList)
{

    if (filterList.empty())
    {
        return {};
    }

    auto re = gt::rex::forFileDialogFilters();
    auto isQtFilter = [&re](const QString& filter)
    {
        return re.match(filter).hasMatch();
    };

    QString result;

    for (int i = 0; i < filterList.size(); ++i)
    {
        auto filter = filterList[i];

        QString delimiter = QStringLiteral(";;");
        if (!isQtFilter(filter))
        {
            if (i < filterList.size()-1 &&
                !isQtFilter(filterList[i+1]))
            {
                delimiter = QStringLiteral("; ");
            }
            filter = QStringLiteral("*") + filter;
        }

        result += filter + delimiter;
    }

    result.chop(2);

    return result;
}
