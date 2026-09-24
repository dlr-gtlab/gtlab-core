/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */
#ifndef GT_MAKEPROPERTYCONNECTION_H
#define GT_MAKEPROPERTYCONNECTION_H

#include "gt_core_exports.h"

#include <QString>

class GtProcessComponent;
class GtPropertyConnection;

namespace gt {
namespace utils {
namespace process {


/**
 * @brief Helper function to create a property connection
 * The property connection is automatically append to the root task. If a nullptr is returned, the connection could not be made.
 * @param sourceComponent Source task or calculator
 * @param sourceProperty ident of the source property
 * @param targetComponent Target task or calculator
 * @param targetProperty ident of the target property
 * @param ok Optional argument for a bool pointer, returns whether the connection could be made
 * @return The new property connection
 */
GT_CORE_EXPORT GtPropertyConnection* makePropertyConnection(GtProcessComponent* sourceComponent,
                                                            const QString &sourceProperty,
                                                            GtProcessComponent* targetComponent,
                                                            const QString &targetProperty,
                                                            bool *ok=nullptr);


} // namespace process
} // namespace utils
} // namespace gt

#endif // GT_MAKEPROPERTYCONNECTION_H
