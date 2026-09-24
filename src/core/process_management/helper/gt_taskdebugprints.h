/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */

#ifndef GT_TASKDEBUGPRINTS_H
#define GT_TASKDEBUGPRINTS_H

#include "gt_core_exports.h"

#include <QString>

class GtProcessComponent;

namespace gt {
namespace debug {
namespace helper {

GT_CORE_EXPORT void printTaskStructure(GtProcessComponent* pcomp, int lvl=0, QString msg=QString());

}}}

#endif // GT_TASKDEBUGPRINTS_H
