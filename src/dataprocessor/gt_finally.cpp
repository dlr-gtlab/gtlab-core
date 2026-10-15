/*
 * GTlab IntelliGraph
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: 2026 German Aerospace Center
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "gt_finally.h"

#include <gt_logging.h>
#include <typeinfo>

void
gt::detail::logCriticalException(const char* origin, const char* what) noexcept
{
    gtFatal().nospace()
        << '\''
        << origin
        << "' caught an unexpected exception!\n"
           "Note:  The application may be in an instable state. Reporting the "
           "issue and restarting the application is adviced!\n"
           "Error: '"
        << (what ? what : "n/a")
        << '\'';
}
