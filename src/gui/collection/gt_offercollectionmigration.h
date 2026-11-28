/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#ifndef GT_OFFERCOLLECTIONMIGRATION_H
#define GT_OFFERCOLLECTIONMIGRATION_H

class GtSettings;

namespace gt
{
namespace detail
{
namespace collectionmigration
{

/**
 * @brief Offer to migrate eligible legacy collections at startup.
 * @param appSettings Application settings used to read and record the migration
 * decision.
 * @return True if startup can proceed without a migration error.
 */
bool offerMigration(GtSettings& appSettings);

} // namespace collectionmigration
} // namespace detail
} // namespace gt

#endif // GT_OFFERCOLLECTIONMIGRATION_H
