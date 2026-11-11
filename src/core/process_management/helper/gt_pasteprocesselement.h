/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Author: Matthias Schuff (SG-VTM)
 */
#ifndef GT_PASTEPROCESSELEMENT_H
#define GT_PASTEPROCESSELEMENT_H

#include <QModelIndex>

#include "gt_core_exports.h"
#include "gt_object.h"

class GtProcessComponent;

namespace gt
{
namespace utils
{
namespace process
{

/**
 * @brief Add a process component as a child to an object using GtObject::appendChild()
 * Checks if the new parent is a process component. If so, transfer the propery connections
 * to the root task
 * @param targetParent
 * @param child The object to be pasted
 * @return
 */
GT_CORE_EXPORT bool pasteProcessComponent(GtObject* targetParent, GtProcessComponent* child);


/**
 * @brief Add a process component as a child to an object using the in-GUI method via
 * GtDataModel function
 * Checks if the new parent is a process component. If so, transfer the propery connections
 * to the root task
 * @param targetParent
 * @param child The object to be pasted
 * @return the index of the pasted process component
 */
GT_CORE_EXPORT QModelIndex pasteProcessComponentToDatamodel(GtObject* targetParent, GtProcessComponent* child, bool* success = nullptr);



} // namespace process
} // namespace utils
} // namespace gt

#endif // GT_PASTEPROCESSELEMENT_H
