/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include <QString>

#include "gt_platformspecifics.h"

std::string
gt::detail::systemUsername()
{
    QString qstr = qEnvironmentVariable("USER");

    if (qstr.isEmpty())
    {
        qstr = qEnvironmentVariable("USERNAME");
    }

    return qstr.toStdString();
}
