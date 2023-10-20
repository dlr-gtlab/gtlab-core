/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 08.06.2022
 *  Author: Martin Siggel (AT-TWK)
 *  Tel.: +49 2203 601 2264
 */

#include "gt_sharedfunction.h"
#include "internal/gt_sharedfunctionhandler.h"

#include "gt_logging.h"

#include <algorithm>

namespace
{
    gt::interface::SharedFunctionID
    divideKey(QString const& functionId)
    {
        QStringList list = functionId.split("::");

        if (list.size() != 2)
        {
            gtWarning() << QObject::tr("Unusual shared function id '%1'. "
                                "Cannot determine module name.").arg(functionId);
            return {"", functionId};
        }

        return {list.first(), list.at(1)};
    }
}

gt::SharedFunction
gt::interface::getSharedFunction(const QString& moduleId,
                            const QString& ident)
{
    return gt::detail::SharedFunctionHandler::instance()
        .getSharedFunction(moduleId, ident);
}

QVector<gt::interface::SharedFunctionID>
gt::interface::sharedFunctionIDs()
{
    auto ids = gt::detail::SharedFunctionHandler::instance()
        .getRegisteredFunctionIDs();

    // separate module and function ID
    QVector<gt::interface::SharedFunctionID> result(ids.size());
    std::transform(ids.begin(), ids.end(), result.begin(), divideKey);

    return result;
}
